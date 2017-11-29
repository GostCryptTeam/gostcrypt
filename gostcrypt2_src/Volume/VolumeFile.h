/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_File
#define GST_HEADER_Platform_File

#include <QFileInfo>
#include <QSharedPointer>
#include "Platform/Buffer.h"

namespace GostCrypt
{
namespace Volume
{
    struct FileType
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


    class VolumeFile
	{
	public:

        VolumeFile () : FileIsOpen (false), preserveTimestamps(false), FileHandle(-1), AccTime(-1), ModTime(-1) { }
        virtual ~VolumeFile ();
			
		void Close ();
        quint32 GetDeviceSectorSize () const;
        QSharedPointer<QFileInfo> GetPath () const;
        quint64 Length () const;
        void Open (const QSharedPointer<QFileInfo> path, bool readOnly, bool preserveTimestamps);
        quint64 Read (const BufferPtr &buffer) const;
        quint64 ReadAt (const BufferPtr &buffer, quint64 position) const;
        void SeekAt (quint64 position) const;
        void SeekEnd (int offset) const;
		void Write (const ConstBufferPtr &buffer) const;
		void Write (const ConstBufferPtr &buffer, size_t length) const { Write (buffer.GetRange (0, length)); }
        void WriteAt (const ConstBufferPtr &buffer, quint64 position) const;
        FileType::Enum GetType () const;
        bool IsTypeFile () const { return GetType() == FileType::File; }
        bool isTypeDevice () const {return (GetType() == FileType::BlockDevice) || (GetType() == FileType::CharacterDevice); }
		
	protected:

		bool FileIsOpen;
        bool preserveTimestamps;
        QSharedPointer<QFileInfo> Path;
        int FileHandle;

		time_t AccTime;
		time_t ModTime;

	private:
        VolumeFile (const VolumeFile &);
        VolumeFile &operator= (const VolumeFile &);
	};
}
}

#endif // GST_HEADER_Platform_File
