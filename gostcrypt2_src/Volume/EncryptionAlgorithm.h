/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionAlgorithm
#define GST_HEADER_Encryption_EncryptionAlgorithm

#include <QList>
#include "CipherAlgorithm.h"
#include "EncryptionMode.h"

namespace GostCrypt
{
namespace Volume {

	class EncryptionAlgorithm;

    typedef QList < QSharedPointer <GostCrypt::Volume::EncryptionAlgorithm> > EncryptionAlgorithmList;

	class EncryptionAlgorithm
	{
	public:
		virtual ~EncryptionAlgorithm ();

		virtual void Decrypt (quint8 *data, quint64 length) const;
		virtual void Decrypt (const BufferPtr &data) const;
		virtual void DecryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		virtual void Encrypt (quint8 *data, quint64 length) const;
		virtual void Encrypt (const BufferPtr &data) const;
		virtual void EncryptSectors (quint8 *data, quint64 sectorIndex, quint64 sectorCount, size_t sectorSize) const;
		static EncryptionAlgorithmList GetAvailableAlgorithms ();
        virtual const CipherAlgorithmList &GetCiphers () const { return Ciphers; }
        virtual QSharedPointer <EncryptionAlgorithm> GetNew () const = 0;
		virtual size_t GetMaxBlockSize () const;
		virtual size_t GetMinBlockSize () const;
		static size_t GetLargestKeySize (const EncryptionAlgorithmList &algorithms);
		virtual size_t GetKeySize () const;
		virtual QSharedPointer <EncryptionMode> GetMode () const;
        virtual std::wstring GetName () const;
        virtual std::wstring GetDescription () const;
		bool IsDeprecated () const { return Deprecated; }
		virtual bool IsModeSupported (const EncryptionMode &mode) const;
		virtual bool IsModeSupported (const QSharedPointer <EncryptionMode> mode) const;
		virtual void SetKey (const ConstBufferPtr &key);
		virtual void SetMode (QSharedPointer <EncryptionMode> mode);

	protected:
		EncryptionAlgorithm ();

		void ValidateState () const;

        CipherAlgorithmList Ciphers;
		bool Deprecated;
		QSharedPointer <EncryptionMode> Mode;
		EncryptionModeList SupportedModes;

	private:
		EncryptionAlgorithm (const EncryptionAlgorithm &);
		EncryptionAlgorithm &operator= (const EncryptionAlgorithm &);
	};
}
}

#endif // GST_HEADER_Encryption_EncryptionAlgorithm
