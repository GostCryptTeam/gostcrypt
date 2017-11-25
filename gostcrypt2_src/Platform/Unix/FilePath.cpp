/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Platform/FilePath.h"
#include <stdio.h>
#include <sys/stat.h>

namespace GostCrypt
{
    void FilePath::Delete () const
	{
        if(!QFile(this->absoluteFilePath()).remove())
            throw; //TODO
	}

    uint FilePath::GetOwner () const
	{
        return this->ownerId();
	}

    FilePathType::Enum FilePath::GetType () const
	{
		struct stat statData;
        if (stat (this->absoluteFilePath().toLocal8Bit().data(), &statData) != 0)
            throw;
		
        if (S_ISREG (statData.st_mode)) return FilePathType::File;
        if (S_ISCHR (statData.st_mode)) return FilePathType::CharacterDevice;
        if (S_ISBLK (statData.st_mode)) return FilePathType::BlockDevice;
        if (S_ISLNK (statData.st_mode)) return FilePathType::SymbolickLink;

        return FilePathType::Unknown;
	}

    QString FilePath::GetBaseName () const
	{
        return this->baseName();
    }

    DevicePath::DevicePath(FilePath filePath) : FilePath(filePath.ToQString())
    {
        if(!filePath.IsDevice())
            throw;
    }

    DevicePath DevicePath::ToHostDriveOfPartition() const
    {
        QString path = this->ToQString();

        bool lastCharacterIsNumber = true;
        path.left(1).toInt(&lastCharacterIsNumber);
        if(!lastCharacterIsNumber)
            throw;//TODO DevicePathIsNotPartition

        return DevicePath(path.right(path.size()-1));
    }

}
