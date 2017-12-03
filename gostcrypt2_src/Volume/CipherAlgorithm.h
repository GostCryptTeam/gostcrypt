/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Ciphers
#define GST_HEADER_Encryption_Ciphers

#include "Platform/Buffer.h"
#include <QList>
#include <QSharedPointer>

namespace GostCrypt
{
namespace Volume {

    class CipherAlgorithm;
    typedef QList < QSharedPointer <CipherAlgorithm> > CipherAlgorithmList;

    class CipherAlgorithm
	{
	public:
        virtual ~CipherAlgorithm ();

		virtual void DecryptBlocks (quint8 *data, size_t blockCount) const;
		virtual void EncryptBlock (quint8 *data) const;
		virtual void EncryptBlocks (quint8 *data, size_t blockCount) const;
		virtual size_t GetBlockSize () const = 0;
		virtual size_t GetKeySize () const = 0;
        virtual std::wstring GetName () const = 0;
        virtual std::wstring GetDescription () const = 0;
        virtual QSharedPointer <CipherAlgorithm> GetNew () const = 0;
		virtual void SetKey (const ConstBufferPtr &key);

		virtual void XorCipherKey (quint8 *ks, quint8 *data, int len) const = 0;
		virtual void EncryptWithKS (quint8 *data, quint8 *ks) const = 0;
		virtual void DecryptWithKS (quint8 *data, quint8 *ks) const = 0;
		virtual void CopyCipherKey (quint8 *ks) const = 0;

		static const int MaxBlockSize = 16;
		virtual size_t GetScheduledKeySize () const = 0;

	protected:
        CipherAlgorithm ();

		virtual void Decrypt (quint8 *data) const = 0;
		virtual void Encrypt (quint8 *data) const = 0;
		virtual void SetCipherKey (const quint8 *key) = 0;

		static bool HwSupportEnabled;
		bool Initialized;
		SecureBuffer Key;
		SecureBuffer ScheduledKey;
		SecureBuffer SwapScheduledKey;

	private:
        CipherAlgorithm (const CipherAlgorithm &);
        CipherAlgorithm &operator= (const CipherAlgorithm &);
	};

}
}
#endif // GST_HEADER_Encryption_Ciphers
