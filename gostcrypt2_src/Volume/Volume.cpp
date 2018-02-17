/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <errno.h>
#include "VolumeException.h"
#include "EncryptionModeXTS.h"
#include "Volume.h"
#include "VolumeHeader.h"
#include "VolumeLayout.h"
#include "Crypto/Crypto.h"

namespace GostCrypt
{
namespace Volume {

	Volume::Volume ()
		: HiddenVolumeProtectionTriggered (false),
        Protection(VolumeProtection::Enum::None),
        SectorSize(0),
        Type (VolumeType::Enum::Unknown),
		VolumeDataSize (0),
		TopWriteOffset (0),
		TotalDataRead (0),
        TotalDataWritten (0)


	{
	}

	Volume::~Volume ()
	{
	}

    void Volume::CheckProtectedRange (quint64 writeHostOffset, quint64 writeLength)
	{
        quint64 writeHostEndOffset = writeHostOffset + writeLength - 1;

		if ((writeHostOffset < ProtectedRangeStart) ? (writeHostEndOffset >= ProtectedRangeStart) : (writeHostOffset <= ProtectedRangeEnd - 1))
		{
			HiddenVolumeProtectionTriggered = true;
            throw VolumeProtectedException();
        }
	}

	void Volume::Close ()
	{
        if (volumeFile.isNull())
            throw VolumeNotOpenException();
        this->volumeFile.reset();
	}

	QSharedPointer <EncryptionAlgorithm> Volume::GetEncryptionAlgorithm () const
	{
		return EA;
	}

    void Volume::Open (const QFileInfo volumePath,
                    bool preserveTimestamps,
                    QSharedPointer <VolumePassword> password,
                    QSharedPointer <KeyfileList> keyfiles,
                    VolumeProtection::Enum protection,
                    QSharedPointer <VolumePassword> protectionPassword, // in case you wqnt to open it as
                    QSharedPointer <KeyfileList> protectionKeyfiles,
                    VolumeType::Enum volumeType,
                    bool useBackupHeaders)
	{
        QSharedPointer<VolumeFile> volumeFile = QSharedPointer<VolumeFile>(new VolumeFile());

        if(protection == VolumeProtection::HiddenVolumeReadOnly) {
            // we first open the hidden volume to get its size
            try
            {
                Volume protectedVolume;

                protectedVolume.Open (volumePath,
                    preserveTimestamps,
                    protectionPassword, protectionKeyfiles,
                    VolumeProtection::ReadOnly,
                    QSharedPointer <VolumePassword> (), QSharedPointer <KeyfileList> (),
                    VolumeType::Hidden,
                    useBackupHeaders);
                if (protectedVolume.GetType() != VolumeType::Hidden)
                    throw IncorrectParameterException("protection is set to HiddenVolumeReadOnly, but the volume type is not hidden.")

                ProtectedRangeStart = protectedVolume.VolumeDataOffset;
                ProtectedRangeEnd = protectedVolume.VolumeDataOffset + protectedVolume.VolumeDataSize;
            }
            catch (PasswordOrKeyfilesIncorrect&)
            {
                throw ProtectionPasswordOrKeyfilesIncorrectException();
            }
        }

        // We First open the file (or device) we want to decrypt
        volumeFile->Open (volumePath, (protection == VolumeProtection::ReadOnly), preserveTimestamps);

		Protection = protection;
        this->volumeFile = volumeFile; // why not directly use the volumeFile from the object ?

		try
		{
            VolumeHostSize = this->volumeFile->Length();
			QSharedPointer <VolumePassword> passwordKey = Keyfile::ApplyListToPassword (keyfiles, password);
			// Test volume layouts
            for (QSharedPointer <VolumeLayout> layout : VolumeLayout::GetAvailableLayouts (volumeType))
			{

				if (useBackupHeaders && !layout->HasBackupHeader())
					continue;

				SecureBuffer headerBuffer (layout->GetHeaderSize());

				int headerOffset = useBackupHeaders ? layout->GetBackupHeaderOffset() : layout->GetHeaderOffset();

				if (headerOffset >= 0)
                    this->volumeFile->SeekAt (headerOffset);
				else
                    this->volumeFile->SeekEnd (headerOffset);

                if (this->volumeFile->Read (headerBuffer) != layout->GetHeaderSize())
					continue;

				EncryptionAlgorithmList layoutEncryptionAlgorithms = layout->GetSupportedEncryptionAlgorithms();

				QSharedPointer <VolumeHeader> header = layout->GetHeader();

                if (header->Decrypt (headerBuffer, *passwordKey, layout->GetSupportedKeyDerivationFunctions(), layoutEncryptionAlgorithms))
				{
					// Header decrypted

					Type = layout->GetType();
					SectorSize = header->GetSectorSize();

					VolumeDataOffset = layout->GetDataOffset (VolumeHostSize);
					VolumeDataSize = layout->GetDataSize (VolumeHostSize);

					Header = header;
					Layout = layout;
					EA = header->GetEncryptionAlgorithm();

					// Volume protection
					if (Protection == VolumeProtection::HiddenVolumeReadOnly)
					{
						if (Type == VolumeType::Hidden)
                            throw PasswordOrKeyfilesIncorrectException(); // the password of the inner volume was put instead of the one of the outer volume.
                        // protectedrangestart and protectedrangeend were set before
					}
					return;
				}
			}
            throw PasswordOrKeyfilesIncorrectException();
		}
		catch (...)
		{
			Close();
            throw; //rethrow
		}
	}

    void Volume::ReadSectors (BufferPtr &buffer, quint64 byteOffset)
	{
        //if_debug (ValidateState ());

        quint64 length = buffer.size();
        quint64 hostOffset = VolumeDataOffset + byteOffset;

		if (length % SectorSize != 0 || byteOffset % SectorSize != 0)
            throw IncorrectParameterException("length or offset not aligned with sector");

        if (this->volumeFile->ReadAt (buffer, hostOffset) != length)
            throw VolumeCorruptedException();

		EA->DecryptSectors (buffer, hostOffset / SectorSize, length / SectorSize, SectorSize);

		TotalDataRead += length;
	}

    void Volume::ReEncryptHeader (bool backupHeader, const BufferPtr &newSalt, const BufferPtr &newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash)
	{
        //if_debug (ValidateState ());

		if (Protection == VolumeProtection::ReadOnly)
            throw VolumeReadOnlyException();

		SecureBuffer newHeaderBuffer (Layout->GetHeaderSize());

        Header->EncryptNew (newHeaderBuffer, newSalt, newHeaderKey, newVolumeHash);

		int headerOffset = backupHeader ? Layout->GetBackupHeaderOffset() : Layout->GetHeaderOffset();

		if (headerOffset >= 0)
            this->volumeFile->SeekAt (headerOffset);
		else
            this->volumeFile->SeekEnd (headerOffset);

        this->volumeFile->Write (newHeaderBuffer);
    }

    void Volume::WriteSectors (const BufferPtr &buffer, quint64 byteOffset)
	{
        quint64 length = buffer.size();
        quint64 hostOffset = VolumeDataOffset + byteOffset;

		if (length % SectorSize != 0
            || byteOffset % SectorSize != 0)
            throw IncorrectParameterException("length or offset not aligned with sector");

        if (byteOffset + length > VolumeDataSize)
            throw IncorrectParameterException("Trying to read after the end of the volume file");

		if (Protection == VolumeProtection::ReadOnly)
            throw VolumeReadOnlyException();
        if (HiddenVolumeProtectionTriggered)//TODO why always throwing when the protection is triggered ?
            throw VolumeProtectedException();

		if (Protection == VolumeProtection::HiddenVolumeReadOnly)
			CheckProtectedRange (hostOffset, length);

		SecureBuffer encBuf (buffer.size());
		encBuf.copyFrom (buffer);

		EA->EncryptSectors (encBuf, hostOffset / SectorSize, length / SectorSize, SectorSize);
        this->volumeFile->WriteAt (encBuf, hostOffset);

		TotalDataWritten += length;

        quint64 writeEndOffset = byteOffset + buffer.size();
		if (writeEndOffset > TopWriteOffset)
			TopWriteOffset = writeEndOffset;
	}

	QSharedPointer<VolumeInformation> Volume::getVolumeInformation()
	{
		QSharedPointer<VolumeInformation> vi = QSharedPointer<VolumeInformation>(new VolumeInformation);

        vi->encryptionAlgorithmName = this->GetEncryptionAlgorithm()->GetName();
		vi->protection = this->Protection;
		vi->size = this->GetSize();
		vi->type = this->Type;
        vi->volumePath = this->volumeFile->GetPath();

                return vi;
	}
}
}
