/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>

#include <sys/mount.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Platform/File.h"
#include <QTextStream>

namespace GostCrypt
{
#if 0
#	define GST_TRACE_FILE_OPERATIONS

	static void TraceFileOperation (int fileHandle, FilePath filePath, bool write, uint64 length, int64 position = -1)
	{
		string path = filePath;
		if (path.empty() || path.find ("gostcrypt_aux_mnt") != string::npos)
			return;

		stringstream s;
		s << path << ": " << (write ? "W " : "R ") << (position == -1 ? lseek (fileHandle, 0, SEEK_CUR) : position) << " (" << length << ")";
		SystemLog::WriteError (s.str());
	}
#endif

	void File::Close ()
	{
        ////if_debug (ValidateState());

		if (!SharedHandle)
		{
			close (FileHandle);
			FileIsOpen = false;

			if ((mFileOpenFlags & File::PreserveTimestamps) && Path.IsFile())
			{
				struct utimbuf u;
				u.actime = AccTime;
				u.modtime = ModTime;

				try
				{
                    if (utime (std::string (Path).c_str(), &u) == -1) // should be in a dedicated method of File.
                        throw;
				}
				catch (...) // Suppress errors to allow using read-only files
				{
#ifdef DEBUG
					throw;
#endif
				}
			}
		}
	}

	void File::Delete ()
	{
		Close();
		Path.Delete();
	}


	void File::Flush () const
	{
//		//if_debug (ValidateState());
        if (fsync (FileHandle) != 0)
            throw;
	}

    quint32 File::GetDeviceSectorSize () const
	{
		if (Path.IsDevice())
		{
			int blockSize;
            if (ioctl (FileHandle, BLKSSZGET, &blockSize) == -1)
                throw;
			return blockSize;
		}
		else
            throw; // (SRC_POS);
	}

    quint64 File::GetPartitionDeviceStartOffset () const
	{
		// HDIO_GETGEO ioctl is limited by the size of long
        DevicePath devicePath = DevicePath(Path);
        QFile file("/sys/block/" + devicePath.ToHostDriveOfPartition().GetBaseName() + "/" + devicePath.GetBaseName() + "/start");

        if(!file.open(QFile::ReadOnly))
            throw;// FailedOpenFileException(QFileInfo(""));
        QByteArray fileContent = file.readAll();
        QTextStream ts(&fileContent);
        quint64 startSectorOffset = ts.readLine().toULongLong();

        return startSectorOffset * GetDeviceSectorSize();
	}

    quint64 File::Length () const
	{
//		//if_debug (ValidateState());

		off_t current = lseek (FileHandle, 0, SEEK_CUR);
        if (current == -1)
            throw;
		SeekEnd (0);
        quint64 length = lseek (FileHandle, 0, SEEK_CUR);
		SeekAt (current);
		return length;
	}

	void File::Open (const FilePath &path, FileOpenMode mode, FileShareMode shareMode, FileOpenFlags flags)
	{
		(void)shareMode;
#ifdef GST_LINUX
		int sysFlags = O_LARGEFILE;
#else
		int sysFlags = 0;
#endif

		switch (mode)
		{
		case CreateReadWrite:
			sysFlags |= O_CREAT | O_TRUNC | O_RDWR;
			break;

		case CreateWrite:
			sysFlags |= O_CREAT | O_TRUNC | O_WRONLY;
			break;

		case OpenRead:
			sysFlags |= O_RDONLY;
			break;

		case OpenWrite:
			sysFlags |= O_WRONLY;
			break;

		case OpenReadWrite:
			sysFlags |= O_RDWR;
			break;

		default:
            throw;// (SRC_POS);
		}

		if ((flags & File::PreserveTimestamps) && path.IsFile())
		{
			struct stat statData;
            if (stat (std::string (path).c_str(), &statData) == -1)
                throw;
			AccTime = statData.st_atime;
			ModTime = statData.st_mtime;
		}

        FileHandle = open (std::string (path).c_str(), sysFlags, S_IRUSR | S_IWUSR);
        if (FileHandle == -1)
            throw;

		Path = path;
		mFileOpenFlags = flags;
		FileIsOpen = true;
	}

    quint64 File::Read (const BufferPtr &buffer) const
	{
        ////if_debug (ValidateState());

#ifdef GST_TRACE_FILE_OPERATIONS
		TraceFileOperation (FileHandle, Path, false, buffer.Size());
#endif
		ssize_t bytesRead = read (FileHandle, buffer, buffer.Size());
        if (bytesRead == -1)
            throw;

		return bytesRead;
	}

    quint64 File::ReadAt (const BufferPtr &buffer, quint64 position) const
	{
        ////if_debug (ValidateState());

#ifdef GST_TRACE_FILE_OPERATIONS
		TraceFileOperation (FileHandle, Path, false, buffer.Size(), position);
#endif
		ssize_t bytesRead = pread (FileHandle, buffer, buffer.Size(), position);
        if (bytesRead == -1)
            throw;

		return bytesRead;
	}

    void File::SeekAt (quint64 position) const
	{
        ////if_debug (ValidateState());
        if (lseek (FileHandle, position, SEEK_SET) == -1)
            throw;
	}

	void File::SeekEnd (int offset) const
	{
        ////if_debug (ValidateState());

        if (lseek (FileHandle, offset, SEEK_END) == -1)
               throw;//TODO
	}

	void File::Write (const ConstBufferPtr &buffer) const
	{
        ////if_debug (ValidateState());

#ifdef GST_TRACE_FILE_OPERATIONS
		TraceFileOperation (FileHandle, Path, true, buffer.Size());
#endif
        if (write (FileHandle, buffer, buffer.Size()) != (ssize_t) buffer.Size())
            throw; //TODO
	}

    void File::WriteAt (const ConstBufferPtr &buffer, quint64 position) const
	{
        ////if_debug (ValidateState());

#ifdef GST_TRACE_FILE_OPERATIONS
		TraceFileOperation (FileHandle, Path, true, buffer.Size(), position);
#endif
        if(pwrite (FileHandle, buffer, buffer.Size(), position) != (ssize_t) buffer.Size())
            throw; //TODO
	}
}
