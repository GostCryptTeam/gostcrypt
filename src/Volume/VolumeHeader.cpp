/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include <QtEndian>
#include "VolumeException.h"
#include "Crc32.h"
#include "EncryptionModeXTS.h"
#include "VolumeHeader.h"
#include "Crypto/Crypto.h"
#include <typeinfo>

namespace GostCrypt
{
namespace Volume
{

VolumeHeader::VolumeHeader(quint32 size)
{
    Init();
    HeaderSize = size;
    EncryptedHeaderDataSize = size - EncryptedHeaderDataOffset;
}

VolumeHeader::~VolumeHeader()
{
    Init();
}

void VolumeHeader::Init()
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

void VolumeHeader::Create(BufferPtr& headerBuffer, VolumeHeaderCreationOptions& options)
{
    if (options.DataKey.size() != options.EA->GetKeySize() * 2)
    {
        throw InvalidParameterException("options.DataKey.size()",
                                        "Given Key size different from the encryption algorithm key size");
    }
    if (options.Salt.size() != GetSaltSize())
    {
        throw InvalidParameterException("options.Salt.size()", "Given salt size incorrect");
    }

    headerBuffer.erase();

    HeaderVersion = CurrentHeaderVersion;
    RequiredMinProgramVersion = CurrentRequiredMinProgramVersion;

    DataAreaKey.erase();
    DataAreaKey.copyFrom(options.DataKey);

    VolumeCreationTime = 0;
    HiddenVolumeDataSize = (options.Type == VolumeType::Hidden ? options.VolumeDataSize :
                            0); // to know if the volume is hidden or not. useless since the layout already contains this information.
    VolumeDataSize = options.VolumeDataSize;

    EncryptedAreaStart = options.VolumeDataStart;
    EncryptedAreaLength = options.VolumeDataSize;

    SectorSize = options.SectorSize;

    if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
            || SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
            || SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
    {
        throw InvalidParameterException("SectorSize",
                                        "The sector size should be greater than " + QString::number(GST_MIN_VOLUME_SECTOR_SIZE) +
                                        ", lower than " + QString::number(GST_MAX_VOLUME_SECTOR_SIZE) + " and multiple of " +
                                        QString::number(ENCRYPTION_DATA_UNIT_SIZE) + ".");
    }

    EA = options.EA;

    EncryptNew(headerBuffer, options.Salt, options.HeaderKey, options.Hash);
}

bool VolumeHeader::Decrypt(const BufferPtr& encryptedData, const VolumePassword& password,
                           const VolumeHashList& keyDerivationFunctions, const EncryptionAlgorithmList& encryptionAlgorithms)
{
    if (password.Size() < 1)
    {
        throw InvalidParameterException("Password", "Empty password");
    }

    const BufferPtr salt(encryptedData.getRange(SaltOffset, SaltSize));
    SecureBuffer header(EncryptedHeaderDataSize);
    SecureBuffer headerKey(GetLargestSerializedKeySize());

    for (QSharedPointer <VolumeHash> derivationfunction : keyDerivationFunctions)
    {
        derivationfunction->HMAC_DeriveKey(headerKey, password, salt);

        for (QSharedPointer <EncryptionAlgorithm> ea : encryptionAlgorithms)
        {
            ea->SetKey(headerKey.getRange(0, ea->GetKeySize()));
            ea->GetMode()->SetKey(headerKey.getRange(ea->GetKeySize(), ea->GetKeySize()));

            header.copyFrom(encryptedData.getRange(EncryptedHeaderDataOffset, EncryptedHeaderDataSize));
            ea->Decrypt(header);

            if (Deserialize(header, ea))
            {
                this->EA = ea;
                this->volumeHash = derivationfunction;
                return true;
            }
        }
    }

    return false;
}


bool VolumeHeader::Deserialize(const BufferPtr& header, QSharedPointer <EncryptionAlgorithm>& ea)
{

    if (header.size() != EncryptedHeaderDataSize)
    {
        throw InvalidParameterException("header.size()", "");
    }

    BufferPtr magicNumber((const quint8*)"TRUE", 4);
    if (!magicNumber.isDataEqual(header.getRange(0, 4)))
    {
        return false;
    }

    /* Vérification de la version du header */
    size_t offset = 4;
    HeaderVersion = DeserializeEntry <quint16> (header, offset);

    if (HeaderVersion < MinAllowedHeaderVersion)
    {
        throw VolumeVersionNotCompatibleException("Volume version is " + QString::number(
                    MinAllowedHeaderVersion, 16) + " and is no longer supported");
    }

    if (HeaderVersion > CurrentHeaderVersion)
    {
        throw VolumeVersionNotCompatibleException("Volume version is " + QString::number(HeaderVersion,
                16) + " but this GostCrypt version support only the version " + QString::number(
                    CurrentHeaderVersion, 16) + " and below.");
    }

    if (HeaderVersion >= 4
            && Crc32::ProcessBuffer(header.getRange(0, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC))
            != DeserializeEntryAt <quint32> (header, GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC))
    {
        return false;
    }

    RequiredMinProgramVersion = DeserializeEntry <quint16> (header, offset);


    if (RequiredMinProgramVersion > VERSION_NUM)
    {
        throw VolumeVersionNotCompatibleException("Volume version too young");
    }

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
    {
        SectorSize = GST_SECTOR_SIZE_LEGACY;
    }

    if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
            || SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
            || SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
    {
        throw InvalidParameterException("SectorSize",
                                        "The sector size should be greater than " + QString::number(GST_MIN_VOLUME_SECTOR_SIZE) +
                                        ", lower than " + QString::number(GST_MAX_VOLUME_SECTOR_SIZE) + " and multiple of " +
                                        QString::number(ENCRYPTION_DATA_UNIT_SIZE) + ".");
    }

    offset = DataAreaKeyOffset;

    if (VolumeKeyAreaCrc32 != Crc32::ProcessBuffer(header.getRange(offset, DataKeyAreaMaxSize)))
    {
        return false;
    }

    DataAreaKey.copyFrom(header.getRange(offset, DataKeyAreaMaxSize));

    //Reset keys for use on the whole volume
    ea->SetKey(header.getRange(offset, ea->GetKeySize()));
    ea->GetMode()->SetKey(header.getRange(offset + ea->GetKeySize(), ea->GetKeySize()));

    return true;
}


template <typename T>
T VolumeHeader::DeserializeEntry(const BufferPtr& header, size_t& offset)
{
    offset += sizeof(T);

    if (offset > header.size())
    {
        throw InvalidParameterException("offset",
                                        "Trying to deserialize header entry after the end of the header");
    }

    return qToBigEndian(*reinterpret_cast<const T*>(header.get() + offset - sizeof(T)));
}

template <typename T>
T VolumeHeader::DeserializeEntryAt(const BufferPtr& header, const size_t& offset)
{
    if (offset + sizeof(T) > header.size())
    {
        throw InvalidParameterException("offset",
                                        "Trying to deserialize header entry after the end of the header");
    }

    return qToBigEndian(*reinterpret_cast<const T*>(header.get() + offset));
}

void VolumeHeader::EncryptNew(BufferPtr& newHeaderBuffer, const BufferPtr& newSalt,
                              const BufferPtr& newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash)
{
    if (newHeaderBuffer.size() != HeaderSize)
    {
        throw InvalidParameterException("ewHeaderBuffer.size()", "Incorrect new header buffer size");
    }
    if (newSalt.size() != SaltSize)
    {
        throw InvalidParameterException("newSalt.size()", "Incorrect new salt size");
    }

    QSharedPointer <EncryptionAlgorithm> ea = EA->GetNew();

    ea->GetMode()->SetKey(newHeaderKey.getRange(ea->GetKeySize(), ea->GetKeySize()));
    ea->SetKey(newHeaderKey.getRange(0, ea->GetKeySize()));

    newHeaderBuffer.copyFrom(newSalt);

    BufferPtr headerData = newHeaderBuffer.getRange(EncryptedHeaderDataOffset, EncryptedHeaderDataSize);
    Serialize(headerData);
    ea->Encrypt(headerData);

    if (newVolumeHash)
    {
        this->volumeHash = newVolumeHash;
    }
}

size_t VolumeHeader::GetLargestSerializedKeySize()
{
    size_t largestKey = EncryptionAlgorithm::GetLargestKeySize(
                            EncryptionAlgorithm::GetAvailableAlgorithms());

    // XTS mode requires the same key size as the encryption algorithm.
    // Legacy modes may require larger key than XTS.
    if (LegacyEncryptionModeKeyAreaSize + largestKey > largestKey * 2)
    {
        return LegacyEncryptionModeKeyAreaSize + largestKey;
    }

    return largestKey * 2;
}

void VolumeHeader::Serialize(BufferPtr& header) const
{
    if (header.size() != EncryptedHeaderDataSize)
    {
        throw InvalidParameterException("header.size()", "Incorrect header buffer size");
    }

    header.erase();

    header[0] = 'T';
    header[1] = 'R';
    header[2] = 'U';
    header[3] = 'E';
    size_t offset = 4;

    header.getRange(DataAreaKeyOffset, DataAreaKey.size()).copyFrom(DataAreaKey);

    quint16 headerVersion = CurrentHeaderVersion;
    SerializeEntry(headerVersion, header, offset);
    SerializeEntry(RequiredMinProgramVersion, header, offset);
    SerializeEntry(Crc32::ProcessBuffer(header.getRange(DataAreaKeyOffset, DataKeyAreaMaxSize)), header,
                   offset);

    quint64 reserved64 = 0;
    SerializeEntry(reserved64, header, offset);
    SerializeEntry(reserved64, header, offset);

    SerializeEntry(HiddenVolumeDataSize, header, offset);
    SerializeEntry(VolumeDataSize, header, offset);
    SerializeEntry(EncryptedAreaStart, header, offset);
    SerializeEntry(EncryptedAreaLength, header, offset);
    SerializeEntry(Flags, header, offset);

    if (SectorSize < GST_MIN_VOLUME_SECTOR_SIZE
            || SectorSize > GST_MAX_VOLUME_SECTOR_SIZE
            || SectorSize % ENCRYPTION_DATA_UNIT_SIZE != 0)
    {
        throw InvalidParameterException("SectorSize",
                                        "The sector size should be greater than " + QString::number(GST_MIN_VOLUME_SECTOR_SIZE) +
                                        ", lower than " + QString::number(GST_MAX_VOLUME_SECTOR_SIZE) + " and multiple of " +
                                        QString::number(ENCRYPTION_DATA_UNIT_SIZE) + ".");
    }

    SerializeEntry(SectorSize, header, offset);

    offset = GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC;
    SerializeEntry(Crc32::ProcessBuffer(header.getRange(0,
                                        GST_HEADER_OFFSET_HEADER_CRC - GST_HEADER_OFFSET_MAGIC)), header, offset);
}

template <typename T>
void VolumeHeader::SerializeEntry(const T& entry, BufferPtr& header, size_t& offset)
{
    offset += sizeof(T);

    if (offset > header.size())
    {
        throw InvalidParameterException("offset",
                                        "Trying to serialize header entry after the end of the header");
    }

    *reinterpret_cast<T*>(header.get() + offset - sizeof(T)) = qToBigEndian(entry);
}
}
}
