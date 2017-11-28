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

#include "VolumeFile.h"
#include <QTextStream>

namespace GostCrypt
{
namespace Volume
{

void VolumeFile::Close ()
	{
        if (!FileIsOpen)
            throw; //TODO

        close (FileHandle);
        FileIsOpen = false;

        if ((preserveTimestamps) && Path.IsFile())
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

    quint32 VolumeFile::GetDeviceSectorSize () const
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

    quint64 VolumeFile::GetPartitionDeviceStartOffset () const
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

    quint64 VolumeFile::Length () const
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

    void VolumeFile::Open (const FilePath &path, bool readOnly, bool preserveTimestamps)
	{
		int sysFlags = O_LARGEFILE;


        if(readOnly)
            sysFlags |= O_RDONLY;
        else
            sysFlags |= O_RDWR;

        if (preserveTimestamps)
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
        preserveTimestamps = preserveTimestamps;
		FileIsOpen = true;
	}

    quint64 VolumeFile::Read (const BufferPtr &buffer) const
	{
        if (!FileIsOpen)
            throw; //TODO
        ssize_t bytesRead = read (FileHandle, buffer, buffer.Size());
        if (bytesRead == -1)
            throw;

		return bytesRead;
	}

    quint64 VolumeFile::ReadAt (const BufferPtr &buffer, quint64 position) const
	{
        if (!FileIsOpen)
            throw; //TODO

		ssize_t bytesRead = pread (FileHandle, buffer, buffer.Size(), position);
        if (bytesRead == -1)
            throw;

		return bytesRead;
	}

    void VolumeFile::SeekAt (quint64 position) const
	{
        if (!FileIsOpen)
            throw; //TODO
        if (lseek (FileHandle, position, SEEK_SET) == -1)
            throw;
	}

    void VolumeFile::SeekEnd (int offset) const
	{
        if (!FileIsOpen)
            throw; //TODO
        if (lseek (FileHandle, offset, SEEK_END) == -1)
               throw;//TODO
	}

    void VolumeFile::Write (const ConstBufferPtr &buffer) const
	{
        if (!FileIsOpen)
            throw; //TODO

        if (write (FileHandle, buffer, buffer.Size()) != (ssize_t) buffer.Size())
            throw; //TODO
	}

    void VolumeFile::WriteAt (const ConstBufferPtr &buffer, quint64 position) const
	{
        if (!FileIsOpen)
            throw; //TODO

        if(pwrite (FileHandle, buffer, buffer.Size(), position) != (ssize_t) buffer.Size())
            throw; //TODO
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


    FilePath VolumeFile::GetPath () const
    {
        return Path;
    }
}
}
