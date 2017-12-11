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
#include "Core/Buffer.h"
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
        virtual void GetDigest (BufferPtr &buffer) = 0;
		virtual size_t GetBlockSize () const = 0;
		virtual size_t GetDigestSize () const = 0;
        virtual std::wstring GetName () const = 0;
		virtual QSharedPointer <VolumeHash> GetNew () const = 0;
		virtual void Init () = 0;
		bool IsDeprecated () const { return Deprecated; }
        virtual void ProcessData (const BufferPtr &data) = 0;

        // derivation functions
        virtual int HMAC_GetIterationCount () const = 0;
        virtual void HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt) const;
        virtual void HMAC_DeriveKey (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount) const = 0;

	protected:
		SecureBuffer Context;
		bool Deprecated;
        static void ValidateKeyDerivationParameters (const BufferPtr &key, const VolumePassword &password, const BufferPtr &salt, int iterationCount);

	private:
		VolumeHash (const VolumeHash &);
		VolumeHash &operator= (const VolumeHash &);
	};

}
}

#endif // GST_HEADER_Encryption_Hash
