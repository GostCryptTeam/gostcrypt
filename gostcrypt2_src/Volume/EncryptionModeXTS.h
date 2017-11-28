/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_EncryptionModeXTS
#define GST_HEADER_Volume_EncryptionModeXTS

#include "EncryptionMode.h"

namespace GostCrypt
{
namespace Volume {

	class EncryptionModeXTS : public EncryptionMode
	{
	public:
		EncryptionModeXTS () { }
		virtual ~EncryptionModeXTS () { }

		virtual void Decrypt (quint8 *data, quint64 length) const;
		virtual void DecryptSectorsCurrentThread (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		virtual void Encrypt (quint8 *data, quint64 length) const;
		virtual void EncryptSectorsCurrentThread (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		virtual const SecureBuffer &GetKey () const { return SecondaryKey; }
		virtual size_t GetKeySize () const;
        virtual std::wstring GetName () const { return L"XTS"; };
		virtual QSharedPointer <EncryptionMode> GetNew () const { return QSharedPointer <EncryptionMode> (new EncryptionModeXTS); }
        virtual void SetCiphers (const CipherAlgorithmList &ciphers);
		virtual void SetKey (const ConstBufferPtr &key);

	protected:
		void DecryptBuffer (quint8 *data, quint64 length, quint64 startDataUnitNo) const;
        void DecryptBufferXTS (const CipherAlgorithm &cipher, const CipherAlgorithm &secondaryCipher, quint8 *buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo) const;
        void DecryptBufferXTS8Byte (const CipherAlgorithm &cipher, const CipherAlgorithm &secondaryCipher, quint8 *buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo) const;
		void EncryptBuffer (quint8 *data, quint64 length, quint64 startDataUnitNo) const;
        void EncryptBufferXTS (const CipherAlgorithm &cipher, const CipherAlgorithm &secondaryCipher, quint8 *buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo) const;
        void EncryptBufferXTS8Byte (const CipherAlgorithm &cipher, const CipherAlgorithm &secondaryCipher, quint8 *buffer, quint64 length, quint64 startDataUnitNo, unsigned int startCipherBlockNo) const;
		void SetSecondaryCipherKeys ();

		SecureBuffer SecondaryKey;
        CipherAlgorithmList SecondaryCiphers;

	private:
		EncryptionModeXTS (const EncryptionModeXTS &);
		EncryptionModeXTS &operator= (const EncryptionModeXTS &);
	};
}
}

#endif // GST_HEADER_Volume_EncryptionModeXTS
