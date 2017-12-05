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

    /**
     * @brief
     *
     */
    class Volume
	{
	public:
		Volume ();
        /**
         * @brief
         *
         */
        virtual ~Volume ();

        /**
         * @brief
         *
         */
        void Close ();
         /**
          * @brief
          *
          * @param volumePath
          * @param preserveTimestamps
          * @param password
          * @param keyfiles
          * @param protection
          * @param protectionPassword
          * @param protectionKeyfiles
          * @param sharedAccessAllowed
          * @param volumeType
          * @param useBackupHeaders
          * @param partitionInSystemEncryptionScope
          *
         * @return QSharedPointer<EncryptionAlgorithm>
         */
        QSharedPointer <EncryptionAlgorithm> GetEncryptionAlgorithm () const;
        /**
         * @brief
         *
         * @return QSharedPointer<VolumeFile>
         */
        QSharedPointer <VolumeFile> GetFile () const { return volumeFile; }
        QSharedPointer <VolumeLayout> GetLayout () const { return Layout; }
        /**
         * @brief
         *
         * @return QSharedPointer<VolumeHeader>
         */
        QSharedPointer <VolumeHeader> GetHeader () const { return Header; }
        /**
         * @brief
         *
         * @return quint64
         */
        quint64 GetHeaderCreationTime () const { return Header->GetHeaderCreationTime(); }
        /**
         * @brief
         *
         * @return const QFileInfo
         */
        const QFileInfo GetPath () const { return volumeFile->GetPath(); }
        /**
         * @brief
         *
         * @return QSharedPointer<VolumeHash>
         */
        QSharedPointer <VolumeHash> GetVolumeHash () const { return Header->GetVolumeHash(); }
        /**
         * @brief
         *
         * @return quint32
         */
        quint32 GetSaltSize () const { return Header->GetSaltSize(); }
        /**
         * @brief
         *
         * @return size_t
         */
        size_t GetSectorSize () const { return SectorSize; }
        /**
         * @brief
         *
         * @return quint64
         */
        quint64 GetSize () const { return VolumeDataSize; }
        /**
         * @brief
         *
         * @return VolumeType::Enum
         */
        VolumeType::Enum GetType () const { return Type; }
        /**
         * @brief
         *
         * @return quint64
         */
        quint64 GetVolumeCreationTime () const { return Header->GetVolumeCreationTime(); }
        /**
         * @brief
         *
         * @param volumePath
         * @param preserveTimestamps
         * @param password
         * @param keyfiles
         * @param protection
         * @param protectionPassword
         * @param protectionKeyfiles
         * @param sharedAccessAllowed
         * @param volumeType
         * @param useBackupHeaders
         * @param partitionInSystemEncryptionScope
         */
        void Open (const QFileInfo volumePath, bool preserveTimestamps, QSharedPointer <VolumePassword> password, QSharedPointer <KeyfileList> keyfiles, VolumeProtection::Enum protection = VolumeProtection::None, QSharedPointer <VolumePassword> protectionPassword = QSharedPointer <VolumePassword> (), QSharedPointer <KeyfileList> protectionKeyfiles = QSharedPointer <KeyfileList> (), bool sharedAccessAllowed = false, VolumeType::Enum volumeType = VolumeType::Unknown, bool useBackupHeaders = false, bool partitionInSystemEncryptionScope = false);
        /**
         * @brief
         *
         * @param buffer
         * @param byteOffset
         */
        void ReadSectors (const BufferPtr &buffer, quint64 byteOffset);
        /**
         * @brief Encrypt the header and write it on the volume
         *
         * @param backupHeader Wether the header should be written at the normal header place or at the backup header place
         * @param newSalt
         * @param newHeaderKey New header cipher key
         * @param newVolumeHash New DerivationFunction that has been used to derive the newHeaderKey from the password/keyfiles
         */
        void ReEncryptHeader (bool backupHeader, const ConstBufferPtr &newSalt, const ConstBufferPtr &newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash);
        /**
         * @brief
         *
         * @param buffer
         * @param byteOffset
         */
        void WriteSectors (const ConstBufferPtr &buffer, quint64 byteOffset);

        /**
         * @brief
         *
         * @return QSharedPointer<VolumeInformation>
         */
        QSharedPointer<VolumeInformation> getVolumeInformation();
	protected:
        /**
         * @brief
         *
         * @param writeHostOffset
         * @param writeLength
         */
        void CheckProtectedRange (quint64 writeHostOffset, quint64 writeLength);

        QSharedPointer <EncryptionAlgorithm> EA; /**< TODO: describe */
        QSharedPointer <VolumeHeader> Header; /**< TODO: describe */
        bool HiddenVolumeProtectionTriggered; /**< TODO: describe */
        QSharedPointer <VolumeLayout> Layout; /**< TODO: describe */
        quint64 ProtectedRangeStart; /**< TODO: describe */
        quint64 ProtectedRangeEnd; /**< TODO: describe */
        VolumeProtection::Enum Protection; /**< TODO: describe */
        size_t SectorSize; /**< TODO: describe */
        bool SystemEncryption; /**< TODO: describe */
        VolumeType::Enum Type; /**< TODO: describe */
        QSharedPointer <VolumeFile> volumeFile; /**< TODO: describe */
        quint64 VolumeHostSize; /**< TODO: describe */
        quint64 VolumeDataOffset; /**< TODO: describe */
        quint64 VolumeDataSize; /**< TODO: describe */
        quint64 TopWriteOffset; /**< TODO: describe */
        quint64 TotalDataRead; /**< TODO: describe */
        quint64 TotalDataWritten; /**< TODO: describe */

	private:
		//Volume (const Volume &);
        /**
         * @brief
         *
         * @param
         * @return Volume &operator
         */
        Volume &operator= (const Volume &);
	};
}
}

#endif // GST_HEADER_Volume_Volume
