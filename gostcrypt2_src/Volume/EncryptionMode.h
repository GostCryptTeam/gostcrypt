/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionMode
#define GST_HEADER_Encryption_EncryptionMode

#include "Common/Crypto.h"
#include "CipherAlgorithm.h"

namespace GostCrypt
{
namespace Volume {

	class EncryptionMode;
    typedef QList < QSharedPointer <EncryptionMode> > EncryptionModeList;

	class EncryptionMode
	{
	public:
		virtual ~EncryptionMode ();

		virtual void Decrypt (quint8 *data, quint64 length) const = 0;
		virtual void DecryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		virtual void DecryptSectorsCurrentThread (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const = 0;
		virtual void Encrypt (quint8 *data, quint64 length) const = 0;
		virtual void EncryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		virtual void EncryptSectorsCurrentThread (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const = 0;
		virtual size_t GetKeySize () const = 0;
        virtual std::wstring GetName () const = 0;
		virtual QSharedPointer <EncryptionMode> GetNew () const = 0;
		virtual void SetKey (const ConstBufferPtr &key) = 0;
        virtual void SetCiphers (const CipherAlgorithmList &ciphers) { Ciphers = ciphers; }

	protected:
		EncryptionMode ();

		static const size_t EncryptionDataUnitSize = ENCRYPTION_DATA_UNIT_SIZE;

        CipherAlgorithmList Ciphers;
		bool KeySet;
		quint64 SectorOffset;

	private:
		EncryptionMode (const EncryptionMode &);
		EncryptionMode &operator= (const EncryptionMode &);
	};
}
}

#endif // GST_HEADER_Encryption_EncryptionMode
