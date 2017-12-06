/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_Volume
#define GST_HEADER_Volume_Volume

#include "EncryptionAlgorithm.h"
#include "EncryptionMode.h"
#include "Keyfile.h"
#include "VolumePassword.h"
#include "VolumeLayout.h"
#include "VolumeInformation.h"
#include "VolumeProtection.h"
#include "VolumeFile.h"

namespace GostCrypt {
namespace Volume {

	class Volume
	{
	public:
		Volume ();
		virtual ~Volume ();

		void Close ();
		QSharedPointer <EncryptionAlgorithm> GetEncryptionAlgorithm () const;
        QSharedPointer <VolumeFile> GetFile () const { return volumeFile; }
		QSharedPointer <VolumeHeader> GetHeader () const { return Header; }
		quint64 GetHeaderCreationTime () const { return Header->GetHeaderCreationTime(); }
        const QFileInfo GetPath () const { return volumeFile->GetPath(); }
        QSharedPointer <VolumeHash> GetVolumeHash () const { return Header->GetVolumeHash(); }
		quint32 GetSaltSize () const { return Header->GetSaltSize(); }
		size_t GetSectorSize () const { return SectorSize; }
		quint64 GetSize () const { return VolumeDataSize; }
		VolumeType::Enum GetType () const { return Type; }
		quint64 GetVolumeCreationTime () const { return Header->GetVolumeCreationTime(); }
        void Open (const QFileInfo volumePath, bool preserveTimestamps, QSharedPointer <VolumePassword> password, QSharedPointer <KeyfileList> keyfiles, VolumeProtection::Enum protection = VolumeProtection::None, QSharedPointer <VolumePassword> protectionPassword = QSharedPointer <VolumePassword> (), QSharedPointer <KeyfileList> protectionKeyfiles = QSharedPointer <KeyfileList> (), bool sharedAccessAllowed = false, VolumeType::Enum volumeType = VolumeType::Unknown, bool useBackupHeaders = false, bool partitionInSystemEncryptionScope = false);
        void ReadSectors (BufferPtr &buffer, quint64 byteOffset);
        void ReEncryptHeader (bool backupHeader, const BufferPtr &newSalt, const BufferPtr &newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash);
        void WriteSectors (const BufferPtr &buffer, quint64 byteOffset);

		QSharedPointer<VolumeInformation> getVolumeInformation();
	protected:
		void CheckProtectedRange (quint64 writeHostOffset, quint64 writeLength);

        QSharedPointer <EncryptionAlgorithm> EA;
		QSharedPointer <VolumeHeader> Header;
		bool HiddenVolumeProtectionTriggered;
		QSharedPointer <VolumeLayout> Layout;
		quint64 ProtectedRangeStart;
		quint64 ProtectedRangeEnd;
		VolumeProtection::Enum Protection;
		size_t SectorSize;
		bool SystemEncryption;
		VolumeType::Enum Type;
        QSharedPointer <VolumeFile> volumeFile;
		quint64 VolumeHostSize;
		quint64 VolumeDataOffset;
		quint64 VolumeDataSize;
		quint64 TopWriteOffset;
		quint64 TotalDataRead;
		quint64 TotalDataWritten;

	private:
		//Volume (const Volume &);
		Volume &operator= (const Volume &);
	};
}
}

#endif // GST_HEADER_Volume_Volume
