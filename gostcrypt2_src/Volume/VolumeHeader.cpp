/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Crc32.h"
#include "EncryptionModeXTS.h"
#include "VolumeHeader.h"
#include "Common/Crypto.h"
#include <typeinfo>

namespace GostCrypt
{
namespace Volume {

	VolumeHeader::VolumeHeader (quint32 size)
	{
		Init();
		HeaderSize = size;
		EncryptedHeaderDataSize = size - EncryptedHeaderDataOffset;
	}

	VolumeHeader::~VolumeHeader ()
	{
		Init();
	}

	void VolumeHeader::Init ()
	{
		VolumeKeyAreaCrc32 = 0;
		VolumeCreationTime = 0;
		HeaderCreationTime = 0;
		mVolumeType = VolumeType::Unknown;
		HiddenVolumeDataSize = 0;
		VolumeDataSize = 0;
		EncryptedAreaStart = 0;
		EncryptedAreaLength = 0;
		Flags = 0;
		SectorSize = 0;
	}

	void VolumeHeader::Create (const BufferPtr &headerBuffer, VolumeHeaderCreationOptions &options)
	{
		if (options.DataKey.Size() != options.EA->GetKeySize() * 2 || options.Salt.Size() != GetSaltSize())
            throw;// ParameterIncorrect (SRC_POS);

		headerBuffer.Zero();

		HeaderVersion = CurrentHeaderVersion;
		RequiredMinProgramVersion = CurrentRequiredMinProgramVersion;

		DataAreaKey.Zero();
		DataAreaKey.CopyFrom (options.DataKey);

		VolumeCreationTime = 0;
        HiddenVolumeDataSize = (options.Type == VolumeType::Hidden ? options.VolumeDataSize : 0); // to know if the volume is hidden or not. useless since the layout already contains this information.
		VolumeDataSize = options.VolumeDataSize;

		EncryptedAreaStart = options.VolumeDataStart;
		EncryptedAreaLength = options.VolumeDataSize;

		SectorSize = options.SectorSize;

		if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
			|| SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
			|| SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
		{
            throw;// ParameterIncorrect (SRC_POS);
		}

		EA = options.EA;
		QSharedPointer <EncryptionMode> mode (new EncryptionModeXTS ());
		EA->SetMode (mode);

        EncryptNew (headerBuffer, options.Salt, options.HeaderKey, options.Hash);
	}

    bool VolumeHeader::Decrypt (const ConstBufferPtr &encryptedData, const VolumePassword &password, const VolumeHashList &keyDerivationFunctions, const EncryptionAlgorithmList &encryptionAlgorithms, const EncryptionModeList &encryptionModes)
	{
		if (password.Size() < 1)
            throw;// PasswordEmpty (SRC_POS);

		ConstBufferPtr salt (encryptedData.GetRange (SaltOffset, SaltSize));
		SecureBuffer header (EncryptedHeaderDataSize);
		SecureBuffer headerKey (GetLargestSerializedKeySize());

        for (QSharedPointer <VolumeHash> derivationfunction : keyDerivationFunctions)
		{
            derivationfunction->DeriveKey (headerKey, password, salt);

            for (QSharedPointer <EncryptionMode> mode : encryptionModes)
			{
				if (typeid (*mode) != typeid (EncryptionModeXTS))
					mode->SetKey (headerKey.GetRange (0, mode->GetKeySize()));

                for (QSharedPointer <EncryptionAlgorithm> ea : encryptionAlgorithms)
				{
					if (!ea->IsModeSupported (mode))
						continue;

					if (typeid (*mode) == typeid (EncryptionModeXTS))
					{
						ea->SetKey (headerKey.GetRange (0, ea->GetKeySize()));

						mode = mode->GetNew();
						mode->SetKey (headerKey.GetRange (ea->GetKeySize(), ea->GetKeySize()));
					}
					else
					{
						ea->SetKey (headerKey.GetRange (LegacyEncryptionModeKeyAreaSize, ea->GetKeySize()));
					}

					ea->SetMode (mode);

					header.CopyFrom (encryptedData.GetRange (EncryptedHeaderDataOffset, EncryptedHeaderDataSize));
					ea->Decrypt (header);

					if (Deserialize (header, ea, mode))
					{
                        this->EA = ea;
                        this->volumeHash = derivationfunction;
						return true;
					}
				}
			}
		}

		return false;
	}


	bool VolumeHeader::Deserialize (const ConstBufferPtr &header, QSharedPointer <EncryptionAlgorithm> &ea, QSharedPointer <EncryptionMode> &mode)
	{

		if (header.Size() != EncryptedHeaderDataSize)
            throw;// ParameterIncorrect (SRC_POS);

		if (header[0] != 'T' ||
			header[1] != 'R' ||
			header[2] != 'U' ||
			header[3] != 'E')
			return false;

/* Vérification de la version du header */
		size_t offset = 4;
		HeaderVersion =	DeserializeEntry <quint16> (header, offset);

		if (HeaderVersion < MinAllowedHeaderVersion)
            throw;// OlderVersionRequired (SRC_POS);

		if (HeaderVersion > CurrentHeaderVersion)
            throw;// HigherVersionRequired (SRC_POS);

		if (HeaderVersion >= 4
			&& Crc32::ProcessBuffer (header.GetRange (0, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC))
			!= DeserializeEntryAt <quint32> (header, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC))
		{
			return false;
		}

		RequiredMinProgramVersion = DeserializeEntry <quint16> (header, offset);


		if (RequiredMinProgramVersion > Version::Number())
            throw;// HigherVersionRequired (SRC_POS);

		VolumeKeyAreaCrc32 = DeserializeEntry <quint32> (header, offset);
		VolumeCreationTime = DeserializeEntry <quint64> (header, offset);
		HeaderCreationTime = DeserializeEntry <quint64> (header, offset);
		HiddenVolumeDataSize = DeserializeEntry <quint64> (header, offset);
		mVolumeType = (HiddenVolumeDataSize != 0 ? VolumeType::Hidden : VolumeType::Normal);
		VolumeDataSize = DeserializeEntry <quint64> (header, offset);
		EncryptedAreaStart = DeserializeEntry <quint64> (header, offset);
		EncryptedAreaLength = DeserializeEntry <quint64> (header, offset);
		Flags = DeserializeEntry <quint32> (header, offset);

		SectorSize = DeserializeEntry <quint32> (header, offset);
		if (HeaderVersion < 5)
			SectorSize = GST_SECTOR_SIZE_LEGACY;

		if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
			|| SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
			|| SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
		{
            throw;// ParameterIncorrect (SRC_POS);
		}

#if !(defined (GST_WINDOWS) || defined (GST_LINUX))
		if (SectorSize != GST_SECTOR_SIZE_LEGACY)
			throw UnsupportedSectorSize (SRC_POS);
#endif

		offset = DataAreaKeyOffset;

		if (VolumeKeyAreaCrc32 != Crc32::ProcessBuffer (header.GetRange (offset, DataKeyAreaMaxSize)))
			return false;

		DataAreaKey.CopyFrom (header.GetRange (offset, DataKeyAreaMaxSize));

		ea = ea->GetNew();
		mode = mode->GetNew();

		if (typeid (*mode) == typeid (EncryptionModeXTS))
		{
			ea->SetKey (header.GetRange (offset, ea->GetKeySize()));
			mode->SetKey (header.GetRange (offset + ea->GetKeySize(), ea->GetKeySize()));
		}
		else
		{
			mode->SetKey (header.GetRange (offset, mode->GetKeySize()));
			ea->SetKey (header.GetRange (offset + LegacyEncryptionModeKeyAreaSize, ea->GetKeySize()));
		}

		ea->SetMode (mode);

		return true;
	}


	template <typename T>
	T VolumeHeader::DeserializeEntry (const ConstBufferPtr &header, size_t &offset) const
	{
		offset += sizeof (T);

		if (offset > header.Size())
            throw;// ParameterIncorrect (SRC_POS);

		return Endian::Big (*reinterpret_cast<const T *> (header.Get() + offset - sizeof (T)));
	}

	template <typename T>
	T VolumeHeader::DeserializeEntryAt (const ConstBufferPtr &header, const size_t &offset) const
	{
		if (offset > header.Size())
            throw;// ParameterIncorrect (SRC_POS);

		return Endian::Big (*reinterpret_cast<const T *> (header.Get() + offset));
	}

    void VolumeHeader::EncryptNew (const BufferPtr &newHeaderBuffer, const ConstBufferPtr &newSalt, const ConstBufferPtr &newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash)
	{
		if (newHeaderBuffer.Size() != HeaderSize || newSalt.Size() != SaltSize)
            throw;// ParameterIncorrect (SRC_POS);

		QSharedPointer <EncryptionMode> mode = EA->GetMode()->GetNew();
		QSharedPointer <EncryptionAlgorithm> ea = EA->GetNew();

		if (typeid (*mode) == typeid (EncryptionModeXTS))
		{
			mode->SetKey (newHeaderKey.GetRange (EA->GetKeySize(), EA->GetKeySize()));
			ea->SetKey (newHeaderKey.GetRange (0, ea->GetKeySize()));
		}
		else
		{
			mode->SetKey (newHeaderKey.GetRange (0, mode->GetKeySize()));
			ea->SetKey (newHeaderKey.GetRange (LegacyEncryptionModeKeyAreaSize, ea->GetKeySize()));
		}

		ea->SetMode (mode);

		newHeaderBuffer.CopyFrom (newSalt);

		BufferPtr headerData = newHeaderBuffer.GetRange (EncryptedHeaderDataOffset, EncryptedHeaderDataSize);
		Serialize (headerData);
		ea->Encrypt (headerData);

        if (newVolumeHash)
            this->volumeHash = newVolumeHash;
	}

	size_t VolumeHeader::GetLargestSerializedKeySize ()
	{
		size_t largestKey = EncryptionAlgorithm::GetLargestKeySize (EncryptionAlgorithm::GetAvailableAlgorithms());

		// XTS mode requires the same key size as the encryption algorithm.
		// Legacy modes may require larger key than XTS.
		if (LegacyEncryptionModeKeyAreaSize + largestKey > largestKey * 2)
			return LegacyEncryptionModeKeyAreaSize + largestKey;

		return largestKey * 2;
	}

	void VolumeHeader::Serialize (const BufferPtr &header) const
	{
		if (header.Size() != EncryptedHeaderDataSize)
            throw;// ParameterIncorrect (SRC_POS);

		header.Zero();

		header[0] = 'T';
		header[1] = 'R';
		header[2] = 'U';
		header[3] = 'E';
		size_t offset = 4;

		header.GetRange (DataAreaKeyOffset, DataAreaKey.Size()).CopyFrom (DataAreaKey);

		quint16 headerVersion = CurrentHeaderVersion;
		SerializeEntry (headerVersion, header, offset);
		SerializeEntry (RequiredMinProgramVersion, header, offset);
		SerializeEntry (Crc32::ProcessBuffer (header.GetRange (DataAreaKeyOffset, DataKeyAreaMaxSize)), header, offset);

		quint64 reserved64 = 0;
		SerializeEntry (reserved64, header, offset);
		SerializeEntry (reserved64, header, offset);

		SerializeEntry (HiddenVolumeDataSize, header, offset);
		SerializeEntry (VolumeDataSize, header, offset);
		SerializeEntry (EncryptedAreaStart, header, offset);
		SerializeEntry (EncryptedAreaLength, header, offset);
		SerializeEntry (Flags, header, offset);

		if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
			|| SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
			|| SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
		{
            throw;// ParameterIncorrect (SRC_POS);
		}

		SerializeEntry (SectorSize, header, offset);

		offset = GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC;
		SerializeEntry (Crc32::ProcessBuffer (header.GetRange (0, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC)), header, offset);
	}

	template <typename T>
	void VolumeHeader::SerializeEntry (const T &entry, const BufferPtr &header, size_t &offset) const
	{
		offset += sizeof (T);

		if (offset > header.Size())
            throw;// ParameterIncorrect (SRC_POS);

		*reinterpret_cast<T *> (header.Get() + offset - sizeof (T)) = Endian::Big (entry);
	}
}
}
