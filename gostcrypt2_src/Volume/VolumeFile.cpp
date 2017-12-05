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

#include "VolumeException.h"
#include "VolumeException.h"
#include "VolumeFile.h"
#include <QTextStream>

namespace GostCrypt
{
namespace Volume
{

void VolumeFile::Close ()
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();

        close (FileHandle);
        FileIsOpen = false;

        if (preserveTimestamps)
            ResetTimestamps();

	}

    quint32 VolumeFile::GetDeviceSectorSize () const
	{
        int blockSize;

        if (!isTypeDevice())
            throw IncorrectParameterException("Volume is not contained in a device");
        if (ioctl (FileHandle, BLKSSZGET, &blockSize) == -1)
            throw FailedGetSectorSizeException();

        return blockSize;
	}

    quint64 VolumeFile::Length () const
	{
		off_t current = lseek (FileHandle, 0, SEEK_CUR);
        if (current == -1)
            throw FailedLseekFileException(Path);
        SeekEnd (0);
        quint64 length = lseek (FileHandle, 0, SEEK_CUR);
		SeekAt (current);
		return length;
	}

    void VolumeFile::Open (const QFileInfo path, bool readOnly, bool preserveTimestamps)
	{
		int sysFlags = O_LARGEFILE;


        if(readOnly)
            sysFlags |= O_RDONLY;
        else
            sysFlags |= O_RDWR;

        if (preserveTimestamps)
		{
			struct stat statData;
            if (stat (path.absoluteFilePath().toLocal8Bit().data(), &statData) == -1)
                throw FailedGetTimestampsException();
			AccTime = statData.st_atime;
			ModTime = statData.st_mtime;
		}

        FileHandle = open (path.absoluteFilePath().toLocal8Bit().data(), sysFlags, S_IRUSR | S_IWUSR);
        if (FileHandle == -1)
            throw FailedOpenFileException(Path);

		Path = path;
        this->preserveTimestamps = preserveTimestamps;
		FileIsOpen = true;
	}

    quint64 VolumeFile::Read (const BufferPtr &buffer) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();
        ssize_t bytesRead = read (FileHandle, buffer, buffer.Size());
        if (bytesRead == -1)
            throw FailedReadFileException(Path);

		return bytesRead;
	}

    quint64 VolumeFile::ReadAt (const BufferPtr &buffer, quint64 position) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();

		ssize_t bytesRead = pread (FileHandle, buffer, buffer.Size(), position);
        if (bytesRead == -1)
            throw FailedReadFileException(Path);

		return bytesRead;
	}

    void VolumeFile::SeekAt (quint64 position) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();
        if (lseek (FileHandle, position, SEEK_SET) == -1)
            throw FailedLseekFileException(Path);
    }

    void VolumeFile::SeekEnd (int offset) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();
        if (lseek (FileHandle, offset, SEEK_END) == -1)
               throw FailedLseekFileException(Path);
	}

    void VolumeFile::Write (const ConstBufferPtr &buffer) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();

        if (write (FileHandle, buffer, buffer.Size()) != (ssize_t) buffer.Size())
            throw FailedWriteFileException(Path)
    }

    void VolumeFile::WriteAt (const ConstBufferPtr &buffer, quint64 position) const
	{
        if (!FileIsOpen)
            throw VolumeNotOpenException();

        if(pwrite (FileHandle, buffer, buffer.Size(), position) != (ssize_t) buffer.Size())
            throw FailedWriteFileException(Path)
    }

    void VolumeFile::Flush() const
    {
        if(fsync(FileHandle) != 0)
            throw FailedFlushFileException(Path);
    }

    FileType::Enum VolumeFile::GetType() const
    {
        struct stat statData;
        if (stat (Path.absoluteFilePath().toLocal8Bit().data(), &statData) != 0)
            throw FailedStatFileException(Path);

        if (S_ISREG (statData.st_mode)) return FileType::File;
        if (S_ISCHR (statData.st_mode)) return FileType::CharacterDevice;
        if (S_ISBLK (statData.st_mode)) return FileType::BlockDevice;
        if (S_ISLNK (statData.st_mode)) return FileType::SymbolickLink;

        return FileType::Unknown;
    }

    void VolumeFile::ResetTimestamps()
    {
        if(!IsTypeFile())
            return;

        struct utimbuf u;
        u.actime = AccTime;
        u.modtime = ModTime;

        try
        {
            if (utime (Path.absoluteFilePath().toLocal8Bit().data(), &u) == -1)
                throw FailedResetTimestampsException();
        }
        catch (FailedResetTimestamps &e) // Suppress errors to allow using read-only files
        {
#ifdef DEBUG
            throw; //rethrow
#endif
        }
    }

    VolumeFile::~VolumeFile ()
    {
        try
        {
            if (FileIsOpen)
                Close();
        }
        catch (...) { }
    }


    const QFileInfo VolumeFile::GetPath () const
    {
        return Path;
    }
}
}
