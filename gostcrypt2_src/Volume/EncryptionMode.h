/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionMode
#define GST_HEADER_Encryption_EncryptionMode

#include "Platform/Platform.h"
#include "Common/Crypto.h"
#include "Cipher.h"

namespace GostCrypt
{
namespace Volume {

	class EncryptionMode;
    typedef std::list < QSharedPointer <EncryptionMode> > EncryptionModeList;

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
		static EncryptionModeList GetAvailableModes ();
        virtual const SecureBuffer &GetKey () const { throw; }
		virtual size_t GetKeySize () const = 0;
        virtual std::wstring GetName () const = 0;
		virtual QSharedPointer <EncryptionMode> GetNew () const = 0;
		virtual quint64 GetSectorOffset () const { return SectorOffset; }
		virtual bool IsKeySet () const { return KeySet; }
		virtual void SetKey (const ConstBufferPtr &key) = 0;
		virtual void SetCiphers (const CipherList &ciphers) { Ciphers = ciphers; }
		virtual void SetSectorOffset (int64 offset) { SectorOffset = offset; }

	protected:
		EncryptionMode ();

		virtual void ValidateState () const;
		void ValidateParameters (quint8 *data, quint64 length) const;
		virtual void ValidateParameters (quint8 *data, quint64 sectorCount, size_t sectorSize) const;

		static const size_t EncryptionDataUnitSize = ENCRYPTION_DATA_UNIT_SIZE;

		CipherList Ciphers;
		bool KeySet;
		quint64 SectorOffset;

	private:
		EncryptionMode (const EncryptionMode &);
		EncryptionMode &operator= (const EncryptionMode &);
	};
}
}

#endif // GST_HEADER_Encryption_EncryptionMode
