/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Keyfile
#define GST_HEADER_Encryption_Keyfile

#include "VolumePassword.h"
#include "Platform/Buffer.h"
#include <QSharedPointer>
#include <QFileInfo>

//move
#define FILE_OPTIMAL_READ_SIZE 256*1024

namespace GostCrypt
{
namespace Volume {

	class Keyfile;
    typedef std::list < QSharedPointer <Keyfile> > KeyfileList;

    class Keyfile
	{
	public:
        explicit Keyfile (const QFileInfo &path) : Path(path) {}
        virtual ~Keyfile () { }

        static QSharedPointer <VolumePassword> ApplyListToPassword (QSharedPointer <KeyfileList> keyfiles, QSharedPointer <VolumePassword> password);

		static const size_t MinProcessedLength = 1;
		static const size_t MaxProcessedLength = 1024 * 1024;

	protected:
		void Apply (const BufferPtr &pool) const;
        QFileInfo Path;

	private:
		Keyfile (const Keyfile &);
		Keyfile &operator= (const Keyfile &);
	};
}
}

#endif // GST_HEADER_Encryption_Keyfile
