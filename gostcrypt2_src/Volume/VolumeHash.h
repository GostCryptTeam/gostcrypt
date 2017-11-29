/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Hash
#define GST_HEADER_Encryption_Hash

#include <QList>
#include <QSharedPointer>
#include "Platform/Buffer.h"
#include "VolumePassword.h"

namespace GostCrypt
{
namespace Volume {

	class VolumeHash;
    typedef QList < QSharedPointer <VolumeHash> > VolumeHashList;

	class VolumeHash
    {
	public:
		VolumeHash () : Deprecated (false) { }
		virtual ~VolumeHash () { }

		static VolumeHashList GetAvailableAlgorithms ();
        virtual void GetDigest (const BufferPtr &buffer) = 0;
		virtual size_t GetBlockSize () const = 0;
		virtual size_t GetDigestSize () const = 0;
        virtual std::wstring GetName () const = 0;
		virtual QSharedPointer <VolumeHash> GetNew () const = 0;
		virtual void Init () = 0;
		bool IsDeprecated () const { return Deprecated; }
		virtual void ProcessData (const ConstBufferPtr &data) = 0;
		virtual void ValidateDataParameters (const ConstBufferPtr &data) const;
		virtual void ValidateDigestParameters (const BufferPtr &buffer) const;

        // derivation functions
        virtual int GetIterationCount () const = 0;
        virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt) const;
        virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const = 0;

	protected:
		SecureBuffer Context;
		bool Deprecated;
        static void ValidateKeyDerivationParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount);

	private:
		VolumeHash (const VolumeHash &);
		VolumeHash &operator= (const VolumeHash &);
	};

}
}

#endif // GST_HEADER_Encryption_Hash
