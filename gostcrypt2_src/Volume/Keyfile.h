/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Keyfile
#define GST_HEADER_Encryption_Keyfile

#include "Platform/Platform.h"
#include "Platform/Stream.h"
#include "VolumePassword.h"
#include <QSharedPointer>

namespace GostCrypt
{
namespace Volume {

	class Keyfile;
    typedef list < QSharedPointer <Keyfile> > KeyfileList;

    class Keyfile //inherit from QFile
	{
	public:
        Keyfile (const FilePath &path){ (void)path; }
        virtual ~Keyfile () { }

        operator FilePath () const { return Path; }
        static QSharedPointer <VolumePassword> ApplyListToPassword (QSharedPointer <KeyfileList> keyfiles, QSharedPointer <VolumePassword> password);
		static QSharedPointer <KeyfileList> DeserializeList (QSharedPointer <Stream> stream, const string &name);
		static void SerializeList (QSharedPointer <Stream> stream, const string &name, QSharedPointer <KeyfileList> keyfiles);
		static bool WasHiddenFilePresentInKeyfilePath() { bool r = HiddenFileWasPresentInKeyfilePath; HiddenFileWasPresentInKeyfilePath = false; return r; }

		static const size_t MinProcessedLength = 1;
		static const size_t MaxProcessedLength = 1024 * 1024;

	protected:
		void Apply (const BufferPtr &pool) const;

		static bool HiddenFileWasPresentInKeyfilePath;

        FilePath Path;

	private:
		Keyfile (const Keyfile &);
		Keyfile &operator= (const Keyfile &);
	};
}
}

#endif // GST_HEADER_Encryption_Keyfile
