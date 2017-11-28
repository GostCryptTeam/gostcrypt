#define FUSE_USE_VERSION  25
#include "FuseService.h"
#include "FuseServiceHandler.h"
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <fuse.h>
#include <signal.h>
#include "Volume/EncryptionThreadPool.h"
#include "Core/CoreBase.h"
#include "Core/CoreException.h"


//TODO handle exception to send back (should be done in Service)
//TODO Handle Logging of exceptions

namespace GostCrypt {
	namespace FuseDriver {

		void FuseService::connectRequestHandlingSignals()
		{
			connect(this, SIGNAL(request(QVariant)), this, SLOT(mountRequestHandler(QVariant)));
			connect(this, SIGNAL(askExit()), this, SIGNAL(exit()));
		}

		void FuseService::initSerializables()
		{
			INIT_SERIALIZE(Core::MountVolumeRequest);
			INIT_SERIALIZE(Core::MountVolumeResponse);
			Core::initCoreException();
			initFuseException();
		}

		void FuseService::mountRequestHandler(QVariant r)
		{
			if(!r.canConvert<QSharedPointer<Core::MountVolumeRequest>>())
				throw UnknowRequestException(r.typeName());

			QSharedPointer<Core::MountVolumeRequest> params = r.value<QSharedPointer<Core::MountVolumeRequest>>();
			QSharedPointer<Core::MountVolumeResponse> response(new Core::MountVolumeResponse());
			mountedVolume.reset(new Volume);

			if(!params->mountedForUser.isEmpty())
				FuseService::userId = Core::getUserId(params->mountedForUser);

			if(!params->mountedForGroup.isEmpty())
				FuseService::groupId = Core::getGroupId(params->mountedForGroup);

			do {
				try {
                    VolumePath path(params->path->absoluteFilePath().toStdWString());
                    shared_ptr<VolumePassword> password;
                    shared_ptr<VolumePassword> protectionPassword;
                    shared_ptr <KeyfileList> keyfiles;
                    shared_ptr <KeyfileList> protectionKeyfiles;


                    // Conversions :(
                    if(!params->password.isNull())
                        password.reset(new VolumePassword(params->password->constData(), params->password->size()));
                    else
                        throw MissingParamException("password");
                    if(!params->protectionPassword.isNull())
                        protectionPassword.reset(new VolumePassword(params->protectionPassword->constData(), params->protectionPassword->size()));
                    if(!params->keyfiles.isNull()) {
                        for(QSharedPointer<QFileInfo> keyfile : *params->keyfiles) {
                            keyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(keyfile->absoluteFilePath().toStdWString()))));
                        }
                    }
                    if(!params->protectionKeyfiles.isNull()) {
                        for(QSharedPointer<QFileInfo> keyfile : *params->protectionKeyfiles) {
                            protectionKeyfiles->push_back(QSharedPointer<Keyfile>(new Keyfile(FilesystemPath(keyfile->absoluteFilePath().toStdWString()))));
                        }
                    }

                    mountedVolume->Open(
						path,
						params->preserveTimestamps,
                        password,
                        keyfiles,
						params->protection,
                        protectionPassword,
                        protectionKeyfiles,
						params->useBackupHeaders
					);
                } catch(GostCrypt::PasswordException &e) {
                    throw IncorrectVolumePasswordException(params->path)
                } catch(GostCrypt::SystemException &e) {
					// In case of permission issue try again in read-only
					if(params->protection != VolumeProtection::ReadOnly && (e.GetErrorCode() == EROFS || e.GetErrorCode() == EACCES || e.GetErrorCode() == EPERM))
					{
						params->protection = VolumeProtection::ReadOnly;
						response->readOnlyFailover = true;
						continue;
					}
                    throw FailedOpenVolumeException(params->path);
                } catch(GostCrypt::Exception &e) {
                    throw ExceptionFromVolumeException(e.what());
                }

				params->password->fill('\0');
                if(!params->protectionPassword.isNull())
                    params->protectionPassword->fill('\0');
				break;
			} while(0);

            try {
                if(params->isDevice)
				{
					if(mountedVolume->GetFile()->GetDeviceSectorSize() != mountedVolume->GetSectorSize())
						throw IncorrectSectorSizeException();
					if(mountedVolume->GetSectorSize() != 512)
						throw IncorrectSectorSizeException();
				}

                fuseMountPoint = params->fuseMountPoint;
                QDir fuseMountPointDir(fuseMountPoint->absoluteFilePath());
                if(!fuseMountPointDir.exists() && !fuseMountPointDir.mkdir(fuseMountPoint->absoluteFilePath()))
                        throw FailedCreateFuseMountPointException(fuseMountPoint);

				try {
                    launchFuse();
					sendResponseWhenReady(QVariant::fromValue(response));
				} catch (...) {
                    QDir(fuseMountPoint->absoluteFilePath()).rmdir(QStringLiteral("."));
					throw;
				}
			} catch (...) {
				mountedVolume->Close();
				throw;
			}
		}

		const char *getVolumeImagePath()
		{
			return "/volume";
		}

		const char *getControlPath()
		{
			return "/control";
		}

		void FuseService::sendResponseWhenReady(QVariant response)
		{
            QSharedPointer<QFileInfo> imageFile(new QFileInfo(fuseMountPoint->absoluteFilePath() + QString(FuseDriver::getVolumeImagePath())));
			for (int t = 0 ; t < 100 ; t++) {
				if(imageFile->exists()) {
					if(QFile(imageFile->absoluteFilePath()).open(QIODevice::ReadWrite)) {
						sendResponse(response);
						return;
					}
				}
				QThread::msleep(100);
			}
			throw FuseTimeoutException(FuseService::volumeInfo->volumePath);
		}

		FuseService::FuseService() : Service("FuseService")
		{
			FuseService::userId = static_cast <gid_t>(0);
            FuseService::groupId = static_cast <gid_t>(0);

            const char *envSudoGID = getenv ("SUDO_GID");
            if (envSudoGID) {
                FuseService::groupId =  static_cast <gid_t> (QString(envSudoGID).toLong());
            }
            const char *envSudoUID = getenv ("SUDO_UID");
            if (envSudoUID) {
                FuseService::userId =  static_cast <gid_t> (QString(envSudoUID).toLong());
			}
		}

		bool FuseService::checkAccessRights()
		{
				return fuse_get_context()->uid == 0 || fuse_get_context()->uid == userId;
		}

		uint64 FuseService::getVolumeSize ()
		{
			if (!mountedVolume)
				throw VolumeNotOpenedYetException();

			return mountedVolume->GetSize();
		}

		uint64 FuseService::getVolumeSectorSize()
		{
			if (!mountedVolume)
				throw VolumeNotOpenedYetException();

			return mountedVolume->GetSectorSize();
		}

		void FuseService::readVolumeSectors(const BufferPtr &buffer, uint64 byteOffset)
		{
			if (!mountedVolume)
				throw VolumeNotOpenedYetException();

			mountedVolume->ReadSectors (buffer, byteOffset);
		}

		void FuseService::writeVolumeSectors(const ConstBufferPtr &buffer, uint64 byteOffset)
		{
			if (!mountedVolume)
				throw VolumeNotOpenedYetException();

			mountedVolume->WriteSectors (buffer, byteOffset);
		}

		bool FuseService::auxDeviceInfoReceived()
		{
			return !FuseService::volumeInfo->virtualDevice.isNull();
		}

		void FuseService::receiveAuxDeviceInfo(QByteArray &buffer)
		{
			QDataStream stream(&buffer, QIODevice::ReadOnly);

			FuseService::volumeInfoMutex.lock();
			stream >> FuseService::volumeInfo->virtualDevice;
			FuseService::volumeInfoMutex.unlock();
		}

		void sendAuxDeviceInfo(QSharedPointer<QFileInfo> fuseMountPoint, QSharedPointer<QFileInfo> virtualDevice)
		{
			QFile controlFile(fuseMountPoint->canonicalFilePath() + getControlPath());

			if(!controlFile.open(QIODevice::ReadWrite)) {
				throw FuseControlFileAccessFailedException(fuseMountPoint)
			}

			QDataStream stream(&controlFile);
			stream << virtualDevice;
		}

		QSharedPointer<QByteArray> FuseService::getVolumeInfo()
		{
			QSharedPointer<QByteArray> buffer(new QByteArray());
			QDataStream stream(buffer.data(), QIODevice::WriteOnly);
			FuseService::volumeInfoMutex.lock();
			stream << FuseService::volumeInfo;
			FuseService::volumeInfoMutex.unlock();


			return buffer;
		}

		int FuseService::exceptionToErrorCode ()
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
				//SystemLog::WriteException (e);
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
				//SystemLog::WriteException (e);
				return -static_cast <int> (e.GetErrorCode());
			}
			catch (std::exception &e)
			{
				//SystemLog::WriteException (e);
				return -EINTR;
			}
			catch (...)
			{
				//SystemLog::WriteException (UnknownException (SRC_POS));
				return -EINTR;
			}
		}

		void FuseService::dismount()
		{
			mountedVolume->Close();
			mountedVolume.reset();

			if (EncryptionThreadPool::IsRunning())
				EncryptionThreadPool::Stop();
		}

		static int fuse_service_access (const char *path, int mask)
		{
			(void)path;
			(void)mask;
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return 0;
		}

		static void *fuse_service_init ()
		{
			try
			{
				// Termination signals are handled by a separate process to allow clean dismount on shutdown
				struct sigaction action;
				memset (&action, 0, sizeof (action));

				action.sa_handler = SIG_IGN;

				sigaction (SIGINT, &action, nullptr);
				sigaction (SIGQUIT, &action, nullptr);
				sigaction (SIGTERM, &action, nullptr);

				if (!EncryptionThreadPool::IsRunning())
					EncryptionThreadPool::Start();
			}
			catch (exception &e)
			{
				//SystemLog::WriteException (e);
			}
			catch (...)
			{
				//SystemLog::WriteException (UnknownException (SRC_POS));
			}

			return nullptr;
		}

		static void fuse_service_destroy (void *userdata)
		{
			(void)userdata;
			try
			{
				FuseService::dismount();
			}
			catch (exception &e)
			{
				//SystemLog::WriteException (e);
			}
			catch (...)
			{
				//SystemLog::WriteException (UnknownException (SRC_POS));
			}
		}

		static int fuse_service_getattr (const char *path, struct stat *statData)
		{
			try
			{
				memset (statData, 0, sizeof(*statData));

				statData->st_uid = FuseService::getUserId();
				statData->st_gid = FuseService::getGroupId();
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
					if (!FuseService::checkAccessRights())
						return -EACCES;

					if (strcmp (path, getVolumeImagePath()) == 0)
					{
						statData->st_mode = S_IFREG | 0600;
						statData->st_nlink = 1;
						statData->st_size = FuseService::getVolumeSize();
					}
					else if (strcmp (path, getControlPath()) == 0)
					{
						statData->st_mode = S_IFREG | 0600;
						statData->st_nlink = 1;
						statData->st_size = FuseService::getVolumeInfo()->size();
					}
					else
					{
						return -ENOENT;
					}
				}
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return 0;
		}

		static int fuse_service_opendir (const char *path, struct fuse_file_info *fi)
		{
			(void)fi;
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;

				if (strcmp (path, "/") != 0)
					return -ENOENT;
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return 0;
		}

		static int fuse_service_open (const char *path, struct fuse_file_info *fi)
		{
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;

				if (strcmp (path, getVolumeImagePath()) == 0)
					return 0;

				if (strcmp (path, getControlPath()) == 0)
				{
					fi->direct_io = 1;
					return 0;
				}
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}
			return -ENOENT;
		}

		static int fuse_service_read (const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
		{
			(void)fi;
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;

				if (strcmp (path, getVolumeImagePath()) == 0)
				{
					try
					{
						// Test for read beyond the end of the volume
						if ((uint64) offset + size > FuseService::getVolumeSize())
							size = FuseService::getVolumeSize() - offset;

						size_t sectorSize = FuseService::getVolumeSectorSize();
						if (size % sectorSize != 0 || offset % sectorSize != 0)
						{
							// Support for non-sector-aligned read operations is required by some loop device tools
							// which may analyze the volume image before attaching it as a device

							uint64 alignedOffset = offset - (offset % sectorSize);
							uint64 alignedSize = size + (offset % sectorSize);

							if (alignedSize % sectorSize != 0)
								alignedSize += sectorSize - (alignedSize % sectorSize);

							SecureBuffer alignedBuffer (alignedSize);

							FuseService::readVolumeSectors(alignedBuffer, alignedOffset);
							BufferPtr ((byte *) buf, size).CopyFrom (alignedBuffer.GetRange (offset % sectorSize, size));
						}
						else
						{
							FuseService::readVolumeSectors(BufferPtr ((byte *) buf, size), offset);
						}
					}
					catch (MissingVolumeData)
					{
						return 0;
					}

					return size;
				}

				if (strcmp (path, getControlPath()) == 0)
				{
					QSharedPointer<QByteArray> infoBuf = FuseService::getVolumeInfo();

					if (offset >= (off_t) infoBuf->size())
						return 0;

					if (offset + size > (unsigned int)infoBuf->size())
						size = infoBuf->size() - offset;

					memcpy(buf, infoBuf->mid(offset, size).data(), size);

					return size;
				}
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return -ENOENT;
		}

		static int fuse_service_readdir (const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
		{
			(void)fi;
			(void)offset;
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;

				if (strcmp (path, "/") != 0)
					return -ENOENT;

				filler (buf, ".", NULL, 0);
				filler (buf, "..", NULL, 0);
				filler (buf, getVolumeImagePath() + 1, NULL, 0);
				filler (buf, getControlPath() + 1, NULL, 0);
			}
			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return 0;
		}

		static int fuse_service_write (const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
		{
			(void)fi;
			try
			{
				if (!FuseService::checkAccessRights())
					return -EACCES;


				if (strcmp (path, getVolumeImagePath()) == 0)
				{
					FuseService::writeVolumeSectors(BufferPtr ((byte *) buf, size), offset);
					return size;
				}

				if (strcmp (path, getControlPath()) == 0)
				{
					if (FuseService::auxDeviceInfoReceived())
						return -EACCES;

					QByteArray buffer(buf, size);
					FuseService::receiveAuxDeviceInfo (buffer);
					return size;
				}
			}

			catch (...)
			{
				return FuseService::exceptionToErrorCode();
			}

			return -ENOENT;
		}
        void FuseService::launchFuse() {
			#ifndef FUSE_SERVICE_DEBUG
			int forkedPid = fork();
			if(forkedPid == -1)
				throw FuseForkFailedException();

			if(!forkedPid) {
				setsid();
			#endif
				VolumeInfo vi;
				vi.Set(*mountedVolume);

				FuseService::volumeInfoMutex.lock();
				FuseService::volumeInfo.reset(new Core::VolumeInformation (vi));
				FuseService::volumeInfo->fuseMountPoint = fuseMountPoint;
				FuseService::volumeInfoMutex.unlock();


				static fuse_operations fuse_service_oper;
				fuse_service_oper.access = fuse_service_access;
				fuse_service_oper.destroy = fuse_service_destroy;
				fuse_service_oper.getattr = fuse_service_getattr;
				fuse_service_oper.init = fuse_service_init;
				fuse_service_oper.open = fuse_service_open;
				fuse_service_oper.opendir = fuse_service_opendir;
				fuse_service_oper.read = fuse_service_read;
				fuse_service_oper.readdir = fuse_service_readdir;
				fuse_service_oper.write = fuse_service_write;

				QByteArray argsByteArray[5];
				argsByteArray[0] = QByteArray("gostcrypt");
				argsByteArray[1] = fuseMountPoint->absoluteFilePath().toLatin1();
				argsByteArray[2] = QByteArray("-o");
				argsByteArray[3] = QByteArray("allow_other");
				argsByteArray[4] = QByteArray("-f");



				char *args[5];
				for(int i = 0; i < 5 ; i++) {
					args[i] = argsByteArray[i].data();
				}

				_exit(fuse_main(5, args, &fuse_service_oper));
				//TODO check that using _exit is memory-clean
			#ifndef FUSE_SERVICE_DEBUG
			}
			#endif
		}

		QSharedPointer<Volume> FuseService::mountedVolume;
		QSharedPointer<Core::VolumeInformation> FuseService::volumeInfo;
        QSharedPointer<QFileInfo> FuseService::fuseMountPoint;
		uid_t FuseService::userId;
		gid_t FuseService::groupId;
		QMutex FuseService::volumeInfoMutex;
	}
}
