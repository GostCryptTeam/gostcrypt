/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_FilesystemPath
#define GST_HEADER_Platform_FilesystemPath

#include "PlatformBase.h"
#include <QSharedPointer>
#include <QFileInfo>

namespace GostCrypt
{
    struct FilePathType
	{
		enum Enum
		{
			Unknown,
            File,
            SymbolickLink,
			BlockDevice,
			CharacterDevice
		};
	};

    class FilePath : protected QFileInfo
	{
	public:
        FilePath () { }
        FilePath (const QString &path) : QFileInfo(path) { }
        FilePath (const wstring &path) : QFileInfo(QString::fromStdWString(path)) { }
        FilePath (const string &path) : QFileInfo(QString::fromStdString(path)) { }

        virtual ~FilePath () { }

        bool operator== (const FilePath &other) const { return *this == other; }
        bool operator!= (const FilePath &other) const { return *this != other; }
        operator wstring () const { return this->absoluteFilePath().toStdWString(); }
        operator string () const { return this->absoluteFilePath().toStdString(); }

		void Delete () const;
        uint GetOwner () const;
        FilePathType::Enum GetType () const;
        bool IsBlockDevice () const { return GetType() == FilePathType::BlockDevice; }
        bool IsCharacterDevice () const { return GetType() == FilePathType::CharacterDevice; }
        bool IsDevice () const { return IsBlockDevice() || IsCharacterDevice(); }
        bool IsEmpty () const { return this->absoluteFilePath().isEmpty(); }
        bool IsFile () const { return GetType() == FilePathType::File; }

        QString GetBaseName () const;
        QString ToQString() const { return this->absoluteFilePath(); }
		
		static const int MaxSize = 260;
	};

    // A remplacer par de l'heritage
    class DevicePath : public FilePath {
    public:
        DevicePath(FilePath filePath);
        DevicePath ToHostDriveOfPartition () const;
    };

    // a remplacer par des listes
    typedef QList < QSharedPointer <FilePath> > FilePathList;
}

#endif // GST_HEADER_Platform_FilesystemPath
