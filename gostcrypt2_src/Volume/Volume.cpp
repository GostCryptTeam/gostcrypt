/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include <errno.h>
#include "EncryptionModeXTS.h"
#include "Volume.h"
#include "VolumeHeader.h"
#include "VolumeLayout.h"
#include "Common/Crypto.h"

namespace GostCrypt
{
namespace Volume {

	Volume::Volume ()
		: HiddenVolumeProtectionTriggered (false),
		SystemEncryption (false),
		VolumeDataSize (0),
		TopWriteOffset (0),
		TotalDataRead (0),
		TotalDataWritten (0)
	{
	}

	Volume::~Volume ()
	{
	}

	void Volume::CheckProtectedRange (uint64 writeHostOffset, uint64 writeLength)
	{
		uint64 writeHostEndOffset = writeHostOffset + writeLength - 1;

		if ((writeHostOffset < ProtectedRangeStart) ? (writeHostEndOffset >= ProtectedRangeStart) : (writeHostOffset <= ProtectedRangeEnd - 1))
		{
			HiddenVolumeProtectionTriggered = true;
            throw;// VolumeProtected (SRC_POS);
		}
	}

	void Volume::Close ()
	{
        if (VolumeFile.isNull())
            throw;// NotInitialized (SRC_POS);

		VolumeFile.reset();
	}

	QSharedPointer <EncryptionAlgorithm> Volume::GetEncryptionAlgorithm () const
	{
		if_debug (ValidateState ());
		return EA;
	}

	QSharedPointer <EncryptionMode> Volume::GetEncryptionMode () const
	{
		if_debug (ValidateState ());
		return EA->GetMode();
	}

    void Volume::Open (const VolumePath &volumePath,
                    bool preserveTimestamps,
                    QSharedPointer <VolumePassword> password,
                    QSharedPointer <KeyfileList> keyfiles,
                    VolumeProtection::Enum protection,
                    QSharedPointer <VolumePassword> protectionPassword, // in case you wqnt to open it as
                    QSharedPointer <KeyfileList> protectionKeyfiles,
                    bool sharedAccessAllowed,
                    VolumeType::Enum volumeType,
                    bool useBackupHeaders,
                    bool partitionInSystemEncryptionScope)
	{
        QSharedPointer<File> volumeFile = QSharedPointer<File>(new File());

		File::FileOpenFlags flags = (preserveTimestamps ? File::PreserveTimestamps : File::FlagsNone);

        if(protection == VolumeProtection::HiddenVolumeReadOnly){
            // we first open the hidden volume to get its size
            //try
            {
                Volume protectedVolume;

                protectedVolume.Open (volumePath,
                    preserveTimestamps,
                    protectionPassword, protectionKeyfiles,
                    VolumeProtection::ReadOnly,
                    QSharedPointer <VolumePassword> (), QSharedPointer <KeyfileList> (),
                    sharedAccessAllowed,
                    VolumeType::Hidden,
                    useBackupHeaders,
                    partitionInSystemEncryptionScope);

                if (protectedVolume.GetType() != VolumeType::Hidden)
                    throw; //ParameterIncorrect (SRC_POS);

                ProtectedRangeStart = protectedVolume.VolumeDataOffset;
                ProtectedRangeEnd = protectedVolume.VolumeDataOffset + protectedVolume.VolumeDataSize;
            }
            /*
            catch (PasswordException&)
            {
                if (protectionKeyfiles && !protectionKeyfiles->empty())
                    throw;// ProtectionPasswordKeyfilesIncorrect (SRC_POS);
                throw;// ProtectionPasswordIncorrect (SRC_POS);
            }//*/
        }

        // We First open the file (or device) we want to decrypt
        //try
		{
			if (protection == VolumeProtection::ReadOnly)
                volumeFile->Open (volumePath, File::OpenRead, File::ShareRead, flags);
			else
                volumeFile->Open (volumePath, File::OpenReadWrite, File::ShareNone, flags);
		}
        /*
        catch (SystemException &e)
		{
			if (e.GetErrorCode() == EAGAIN)
			{
				if (!sharedAccessAllowed)
					throw VolumeHostInUse (SRC_POS);

                volumeFile->Open (volumePath, protection == VolumeProtection::ReadOnly ? File::OpenRead : File::OpenReadWrite, File::ShareReadWriteIgnoreLock, flags);
			}
			else
				throw;
        }//*/

		if (!volumeFile)
            throw;// ParameterIncorrect (SRC_POS); // TODO : wrong error

		Protection = protection;
		VolumeFile = volumeFile;
		SystemEncryption = partitionInSystemEncryptionScope;

		try
		{
			VolumeHostSize = VolumeFile->Length();
			QSharedPointer <VolumePassword> passwordKey = Keyfile::ApplyListToPassword (keyfiles, password);

			bool deviceHosted = GetPath().IsDevice();
			size_t hostDeviceSectorSize = 0;
			if (deviceHosted)
				hostDeviceSectorSize = volumeFile->GetDeviceSectorSize();

			// Test volume layouts
            for (QSharedPointer <VolumeLayout> layout : VolumeLayout::GetAvailableLayouts (volumeType))
			{

				if (useBackupHeaders && !layout->HasBackupHeader())
					continue;

				SecureBuffer headerBuffer (layout->GetHeaderSize());

				if (partitionInSystemEncryptionScope)
					continue;

				int headerOffset = useBackupHeaders ? layout->GetBackupHeaderOffset() : layout->GetHeaderOffset();

				if (headerOffset >= 0)
					VolumeFile->SeekAt (headerOffset);
				else
					VolumeFile->SeekEnd (headerOffset);

				if (VolumeFile->Read (headerBuffer) != layout->GetHeaderSize())
					continue;

				EncryptionAlgorithmList layoutEncryptionAlgorithms = layout->GetSupportedEncryptionAlgorithms();
				EncryptionModeList layoutEncryptionModes = layout->GetSupportedEncryptionModes();

				QSharedPointer <VolumeHeader> header = layout->GetHeader();

				if (header->Decrypt (headerBuffer, *passwordKey, layout->GetSupportedKeyDerivationFunctions(), layoutEncryptionAlgorithms, layoutEncryptionModes))
				{
					// Header decrypted

					Type = layout->GetType();
					SectorSize = header->GetSectorSize();

					VolumeDataOffset = layout->GetDataOffset (VolumeHostSize);
					VolumeDataSize = layout->GetDataSize (VolumeHostSize);

					Header = header;
					Layout = layout;
					EA = header->GetEncryptionAlgorithm();
					EncryptionMode &mode = *EA->GetMode();

					// Volume protection
					if (Protection == VolumeProtection::HiddenVolumeReadOnly)
					{
						if (Type == VolumeType::Hidden)
                            throw;// PasswordIncorrect (SRC_POS); // the password of the inner volume was put instead of the one of the outer volume.
                        // protectedrangestart and protectedrangeend were set before
					}
					return;
				}
			}

			if (keyfiles && !keyfiles->empty())
                throw;// PasswordKeyfilesIncorrect (SRC_POS);
            throw;// PasswordIncorrect (SRC_POS);
		}
		catch (...)
		{
			Close();
			throw;
		}
	}

	void Volume::ReadSectors (const BufferPtr &buffer, uint64 byteOffset)
	{
		if_debug (ValidateState ());

		uint64 length = buffer.Size();
		uint64 hostOffset = VolumeDataOffset + byteOffset;

		if (length % SectorSize != 0 || byteOffset % SectorSize != 0)
            throw;// ParameterIncorrect (SRC_POS);

		if (VolumeFile->ReadAt (buffer, hostOffset) != length)
            throw;// MissingVolumeData (SRC_POS);

		EA->DecryptSectors (buffer, hostOffset / SectorSize, length / SectorSize, SectorSize);

		TotalDataRead += length;
	}

	void Volume::ReEncryptHeader (bool backupHeader, const ConstBufferPtr &newSalt, const ConstBufferPtr &newHeaderKey, QSharedPointer <Pkcs5Kdf> newPkcs5Kdf)
	{
		if_debug (ValidateState ());

		if (Protection == VolumeProtection::ReadOnly)
            throw;// VolumeReadOnly (SRC_POS);

		SecureBuffer newHeaderBuffer (Layout->GetHeaderSize());

		Header->EncryptNew (newHeaderBuffer, newSalt, newHeaderKey, newPkcs5Kdf);

		int headerOffset = backupHeader ? Layout->GetBackupHeaderOffset() : Layout->GetHeaderOffset();

		if (headerOffset >= 0)
			VolumeFile->SeekAt (headerOffset);
		else
			VolumeFile->SeekEnd (headerOffset);

		VolumeFile->Write (newHeaderBuffer);
	}

	void Volume::ValidateState () const
	{
        if (VolumeFile.isNull())
            throw;// NotInitialized (SRC_POS);
	}

	void Volume::WriteSectors (const ConstBufferPtr &buffer, uint64 byteOffset)
	{
		if_debug (ValidateState ());

		uint64 length = buffer.Size();
		uint64 hostOffset = VolumeDataOffset + byteOffset;

		if (length % SectorSize != 0
			|| byteOffset % SectorSize != 0
			|| byteOffset + length > VolumeDataSize)
            throw;// ParameterIncorrect (SRC_POS);

		if (Protection == VolumeProtection::ReadOnly)
            throw;// VolumeReadOnly (SRC_POS);

		if (HiddenVolumeProtectionTriggered)
            throw;// VolumeProtected (SRC_POS);

		if (Protection == VolumeProtection::HiddenVolumeReadOnly)
			CheckProtectedRange (hostOffset, length);

		SecureBuffer encBuf (buffer.Size());
		encBuf.CopyFrom (buffer);

		EA->EncryptSectors (encBuf, hostOffset / SectorSize, length / SectorSize, SectorSize);
		VolumeFile->WriteAt (encBuf, hostOffset);

		TotalDataWritten += length;

		uint64 writeEndOffset = byteOffset + buffer.Size();
		if (writeEndOffset > TopWriteOffset)
			TopWriteOffset = writeEndOffset;
	}

	QSharedPointer<VolumeInformation> Volume::getVolumeInformation()
	{
		QSharedPointer<VolumeInformation> vi = QSharedPointer<VolumeInformation>(new VolumeInformation);

		vi->encryptionAlgorithmName = QString::fromStdWString(this->GetEncryptionAlgorithm()->GetName());
		vi->protection = this->Protection;
		vi->size = this->GetSize();
		vi->type = this->Type;
		vi->volumePath.reset(new QFileInfo( QString::fromStdWString(wstring(this->VolumeFile->GetPath()))));
	}
}
}
