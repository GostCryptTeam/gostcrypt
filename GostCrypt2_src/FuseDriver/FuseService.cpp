/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#define FUSE_USE_VERSION  25
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>

#include "FuseService.h"
#include "Platform/FileStream.h"
#include "Platform/MemoryStream.h"
#include "Platform/Serializable.h"
#include "Platform/SystemLog.h"
#include "Platform/Unix/Pipe.h"
#include "Platform/Unix/Poller.h"
#include "Volume/EncryptionThreadPool.h"


/*
 * Contrairement à ce que je pensais au départ, on utilise fuse non pas pour monter le système de fichier final dans le système (ce qui nous permettrais a priori de ne pas avoir besoin des privileges), mais pour pouvoir gérer le chiffrement.
 * Dans le système de fichier fuse dev ici, il y a que deux fichier dont un représente le volume. L'interet de fuse c'est qu'a chaque fois qu'on veut lire/écrire dans le fichier volume le système de fichier dev dans ce fichier sera appelé. Les fonction de lecture et écritures vont faire appel au chiffrement déchifrement pour dechifréer que la partie qui nous interesse dans le dossier
 * Je pense que l'étape qui conciste à transformer ce gros fichier volume (maintenant déchiffrer on the fly) en système de fichier linux est gérer par les loopback qui est une fonctionalité linux dédié à cela si j'ai bien compris */

namespace GostCrypt
{
	static int fuse_service_access (const char *path, int mask)
	{
        (void)path;
        (void)mask;
        try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return 0;
	}

	static void *fuse_service_init ()
	{
		try
		{
			// Termination signals are handled by a separate process to allow clean dismount on shutdown
			struct sigaction action;
			Memory::Zero (&action, sizeof (action));
			action.sa_handler = SIG_IGN;

			sigaction (SIGINT, &action, nullptr);
			sigaction (SIGQUIT, &action, nullptr);
			sigaction (SIGTERM, &action, nullptr);

			if (!EncryptionThreadPool::IsRunning())
				EncryptionThreadPool::Start();
		}
		catch (exception &e)
		{
			SystemLog::WriteException (e);
		}
		catch (...)
		{
			SystemLog::WriteException (UnknownException (SRC_POS));
		}

		return nullptr;
	}

	static void fuse_service_destroy (void *userdata)
	{
        (void)userdata;
        try
		{
			FuseService::Dismount();
		}
		catch (exception &e)
		{
			SystemLog::WriteException (e);
		}
		catch (...)
		{
			SystemLog::WriteException (UnknownException (SRC_POS));
		}
	}

	static int fuse_service_getattr (const char *path, struct stat *statData)
	{
		try
		{
			Memory::Zero (statData, sizeof(*statData));

			statData->st_uid = FuseService::GetUserId();
			statData->st_gid = FuseService::GetGroupId();
			statData->st_atime = time (NULL);
			statData->st_ctime = time (NULL);
			statData->st_mtime = time (NULL);

			if (strcmp (path, "/") == 0)
			{
				statData->st_mode = S_IFDIR | 0500;
				statData->st_nlink = 2;
			}
			else
			{
				if (!FuseService::CheckAccessRights())
					return -EACCES;

				if (strcmp (path, FuseService::GetVolumeImagePath()) == 0)
				{
					statData->st_mode = S_IFREG | 0600;
					statData->st_nlink = 1;
					statData->st_size = FuseService::GetVolumeSize();
				}
				else if (strcmp (path, FuseService::GetControlPath()) == 0)
				{
					statData->st_mode = S_IFREG | 0600;
					statData->st_nlink = 1;
					statData->st_size = FuseService::GetVolumeInfo()->Size();
				}
				else
				{
					return -ENOENT;
				}
			}
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return 0;
	}

	static int fuse_service_opendir (const char *path, struct fuse_file_info *fi)
	{
        (void)fi;
        try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;

			if (strcmp (path, "/") != 0)
				return -ENOENT;
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return 0;
	}

	static int fuse_service_open (const char *path, struct fuse_file_info *fi)
	{
		try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;

			if (strcmp (path, FuseService::GetVolumeImagePath()) == 0)
				return 0;

			if (strcmp (path, FuseService::GetControlPath()) == 0)
			{
				fi->direct_io = 1;
				return 0;
			}
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}
		return -ENOENT;
	}

	static int fuse_service_read (const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
	{
        (void)fi;
        try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;

			if (strcmp (path, FuseService::GetVolumeImagePath()) == 0)
			{
				try
				{
					// Test for read beyond the end of the volume
					if ((uint64) offset + size > FuseService::GetVolumeSize())
						size = FuseService::GetVolumeSize() - offset;

					size_t sectorSize = FuseService::GetVolumeSectorSize();
					if (size % sectorSize != 0 || offset % sectorSize != 0)
					{
						// Support for non-sector-aligned read operations is required by some loop device tools
						// which may analyze the volume image before attaching it as a device

						uint64 alignedOffset = offset - (offset % sectorSize);
						uint64 alignedSize = size + (offset % sectorSize);

						if (alignedSize % sectorSize != 0)
							alignedSize += sectorSize - (alignedSize % sectorSize);

						SecureBuffer alignedBuffer (alignedSize);

						FuseService::ReadVolumeSectors (alignedBuffer, alignedOffset);
						BufferPtr ((byte *) buf, size).CopyFrom (alignedBuffer.GetRange (offset % sectorSize, size));
					}
					else
					{
						FuseService::ReadVolumeSectors (BufferPtr ((byte *) buf, size), offset);
					}
				}
				catch (MissingVolumeData)
				{
					return 0;
				}

				return size;
			}

			if (strcmp (path, FuseService::GetControlPath()) == 0)
			{
				shared_ptr <Buffer> infoBuf = FuseService::GetVolumeInfo();
				BufferPtr outBuf ((byte *)buf, size);

				if (offset >= (off_t) infoBuf->Size())
					return 0;

				if (offset + size > infoBuf->Size())
					size = infoBuf->Size () - offset;

				outBuf.CopyFrom (infoBuf->GetRange (offset, size));
				return size;
			}
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return -ENOENT;
	}

	static int fuse_service_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
	{
        (void)fi;
        (void)offset;
        try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;

			if (strcmp (path, "/") != 0)
				return -ENOENT;

			filler (buf, ".", NULL, 0);
			filler (buf, "..", NULL, 0);
			filler (buf, FuseService::GetVolumeImagePath() + 1, NULL, 0);
			filler (buf, FuseService::GetControlPath() + 1, NULL, 0);
		}
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return 0;
	}

	static int fuse_service_write (const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
	{
        (void)fi;
        try
		{
			if (!FuseService::CheckAccessRights())
				return -EACCES;

			if (strcmp (path, FuseService::GetVolumeImagePath()) == 0)
			{
				FuseService::WriteVolumeSectors (BufferPtr ((byte *) buf, size), offset);
				return size;
			}

			if (strcmp (path, FuseService::GetControlPath()) == 0)
			{
				if (FuseService::AuxDeviceInfoReceived())
					return -EACCES;

				FuseService::ReceiveAuxDeviceInfo (ConstBufferPtr ((const byte *)buf, size));
				return size;
			}
		}
#ifdef GST_FREEBSD
		// FreeBSD apparently retries failed write operations forever, which may lead to a system crash.
		catch (VolumeReadOnly&)
		{
			return size;
		}
		catch (VolumeProtected&)
		{
			return size;
		}
#endif
		catch (...)
		{
			return FuseService::ExceptionToErrorCode();
		}

		return -ENOENT;
	}

	bool FuseService::CheckAccessRights ()
	{
		return fuse_get_context()->uid == 0 || fuse_get_context()->uid == UserId;
	}

	void FuseService::CloseMountedVolume ()
	{
		if (MountedVolume)
		{
			// This process will exit before the use count of MountedVolume reaches zero
            //if (MountedVolume->GetFile().use_count() > 1)
				MountedVolume->GetFile()->Close();

            /*if (MountedVolume.use_count() > 1)
                delete MountedVolume.get();*/

			MountedVolume.reset();
		}
	}

	void FuseService::Dismount ()
	{
		CloseMountedVolume();

		if (EncryptionThreadPool::IsRunning())
			EncryptionThreadPool::Stop();
	}

	int FuseService::ExceptionToErrorCode ()
	{
		try
		{
			throw;
		}
		catch (std::bad_alloc)
		{
			return -ENOMEM;
		}
		catch (ParameterIncorrect &e)
		{
			SystemLog::WriteException (e);
			return -EINVAL;
		}
		catch (VolumeProtected&)
		{
			return -EPERM;
		}
		catch (VolumeReadOnly&)
		{
			return -EPERM;
		}
		catch (SystemException &e)
		{
			SystemLog::WriteException (e);
			return -static_cast <int> (e.GetErrorCode());
		}
		catch (std::exception &e)
		{
			SystemLog::WriteException (e);
			return -EINTR;
		}
		catch (...)
		{
			SystemLog::WriteException (UnknownException (SRC_POS));
			return -EINTR;
		}
	}

	shared_ptr <Buffer> FuseService::GetVolumeInfo ()
	{
		shared_ptr <Stream> stream (new MemoryStream);

		{
			ScopeLock lock (OpenVolumeInfoMutex);

			OpenVolumeInfo.Set (*MountedVolume);
			OpenVolumeInfo.SlotNumber = SlotNumber;

			OpenVolumeInfo.Serialize (stream);
		}

		ConstBufferPtr infoBuf = dynamic_cast <MemoryStream&> (*stream);
		shared_ptr <Buffer> outBuf (new Buffer (infoBuf.Size()));
		outBuf->CopyFrom (infoBuf);

		return outBuf;
	}

	const char *FuseService::GetVolumeImagePath ()
	{
#ifdef GST_MACOSX
		return "/volume.dmg";
#else
		return "/volume";
#endif
	}

	uint64 FuseService::GetVolumeSize ()
	{
		if (!MountedVolume)
			throw NotInitialized (SRC_POS);

		return MountedVolume->GetSize();
	}

	void FuseService::Mount (shared_ptr <Volume> openVolume, VolumeSlotNumber slotNumber, const string &fuseMountPoint)
	{
		list <string> args;
		args.push_back (FuseService::GetDeviceType());
		args.push_back (fuseMountPoint);

#ifdef GST_MACOSX
		args.push_back ("-o");
		args.push_back ("noping_diskarb");
		args.push_back ("-o");
		args.push_back ("nobrowse");

		if (getuid() == 0 || geteuid() == 0)
#endif
		{
			args.push_back ("-o");
			args.push_back ("allow_other");
		}

		ExecFunctor execFunctor (openVolume, slotNumber);
		Process::Execute ("fuse", args, -1, &execFunctor);

		for (int t = 0; true; t++)
		{
			try
			{
				if (FilesystemPath (fuseMountPoint + FuseService::GetControlPath()).GetType() == FilesystemPathType::File)
					break;
			}
			catch (...)
			{
				if (t > 50)
					throw;

				Thread::Sleep (100);
			}
		}
	}

	void FuseService::ReadVolumeSectors (const BufferPtr &buffer, uint64 byteOffset)
	{
		if (!MountedVolume)
			throw NotInitialized (SRC_POS);

		MountedVolume->ReadSectors (buffer, byteOffset);
	}

	void FuseService::ReceiveAuxDeviceInfo (const ConstBufferPtr &buffer)
	{
		shared_ptr <Stream> stream (new MemoryStream (buffer));
		Serializer sr (stream);

		ScopeLock lock (OpenVolumeInfoMutex);
		OpenVolumeInfo.VirtualDevice = sr.DeserializeString ("VirtualDevice");
		OpenVolumeInfo.LoopDevice = sr.DeserializeString ("LoopDevice");
	}

	void FuseService::SendAuxDeviceInfo (const DirectoryPath &fuseMountPoint, const DevicePath &virtualDevice, const DevicePath &loopDevice)
	{
		File fuseServiceControl;
		fuseServiceControl.Open (string (fuseMountPoint) + GetControlPath(), File::OpenWrite);

		shared_ptr <Stream> stream (new MemoryStream);
		Serializer sr (stream);

		sr.Serialize ("VirtualDevice", string (virtualDevice));
		sr.Serialize ("LoopDevice", string (loopDevice));
		fuseServiceControl.Write (dynamic_cast <MemoryStream&> (*stream));
	}

	void FuseService::WriteVolumeSectors (const ConstBufferPtr &buffer, uint64 byteOffset)
	{
		if (!MountedVolume)
			throw NotInitialized (SRC_POS);

		MountedVolume->WriteSectors (buffer, byteOffset);
	}

	void FuseService::OnSignal (int signal)
	{
        (void)signal;
        try
		{
			//shared_ptr <VolumeInfo> volume = Core->GetMountedVolume (SlotNumber);

			//if (volume)
				//Core->DismountVolume (volume, true);
		}
		catch (...) { }

		_exit (0);
	}

	void FuseService::ExecFunctor::operator() (int argc, char *argv[])
	{
		struct timeval tv;
		gettimeofday (&tv, NULL);
		FuseService::OpenVolumeInfo.SerialInstanceNumber = (uint64)tv.tv_sec * 1000000ULL + tv.tv_usec;

		FuseService::MountedVolume = MountedVolume;
		FuseService::SlotNumber = SlotNumber;

		FuseService::UserId = getuid();
		FuseService::GroupId = getgid();

                // SUDO_UID est l'UID de l'utilisateur qui a lancé le SUDO, donc la on test si l'utilisateur qui a lancé le sudo est different de root (UID 0)
                if (getenv ("SUDO_UID"))
		{
			try
			{
				string s (getenv ("SUDO_UID"));
				FuseService::UserId = static_cast <uid_t> (StringConverter::ToUInt64 (s));

				if (getenv ("SUDO_GID"))
				{
					s = getenv ("SUDO_GID");
					FuseService::GroupId = static_cast <gid_t> (StringConverter::ToUInt64 (s));
				}
			}
			catch (...) { }
		}
		static fuse_operations fuse_service_oper;

                // Ici on bind toutes les fonctions statiques pour les passer à la library fuse
                fuse_service_oper.access = fuse_service_access;
		fuse_service_oper.destroy = fuse_service_destroy;
		fuse_service_oper.getattr = fuse_service_getattr;
		fuse_service_oper.init = fuse_service_init;
		fuse_service_oper.open = fuse_service_open;
		fuse_service_oper.opendir = fuse_service_opendir;
		fuse_service_oper.read = fuse_service_read;
		fuse_service_oper.readdir = fuse_service_readdir;
		fuse_service_oper.write = fuse_service_write;

		// Create a new session
		setsid ();

		// Fork handler of termination signals
		SignalHandlerPipe.reset (new Pipe);

		int forkedPid = fork();
		throw_sys_if (forkedPid == -1);

		if (forkedPid == 0)
		{
			CloseMountedVolume();

			struct sigaction action;
			Memory::Zero (&action, sizeof (action));
			action.sa_handler = OnSignal;

			sigaction (SIGINT, &action, nullptr);
			sigaction (SIGQUIT, &action, nullptr);
			sigaction (SIGTERM, &action, nullptr);

			// Wait for the exit of the main service
			byte buf[1];
                        // La lecture va atteindre d'avoir quelques chose à lire, mais rien n'est envoyé. Lorsque la pipe est fermé en écriture par le parent le read va donc se débloquer
			if (read (SignalHandlerPipe->GetReadFD(), buf, sizeof (buf))) { } // Errors ignored

			_exit (0);
		}

                //On ferme la pipe en lecture
                SignalHandlerPipe->GetWriteFD();


		_exit (fuse_main (argc, argv, &fuse_service_oper));
                //une fois que le process enfant est terminé la pipe sera automatiquement fermé et le processus parent va se fermé
	}

	VolumeInfo FuseService::OpenVolumeInfo;
	Mutex FuseService::OpenVolumeInfoMutex;
	shared_ptr <Volume> FuseService::MountedVolume;
	VolumeSlotNumber FuseService::SlotNumber;
	uid_t FuseService::UserId;
	gid_t FuseService::GroupId;
	unique_ptr <Pipe> FuseService::SignalHandlerPipe;
}