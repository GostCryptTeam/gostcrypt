/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_VolumeHeader
#define GST_HEADER_Volume_VolumeHeader

#include "Crypto/Gstdefs.h"
#include "Crypto/Volumes.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Volume/EncryptionMode.h"
#include "Volume/Keyfile.h"
#include "Volume/VolumePassword.h"
#include "Volume/VolumeHash.h"
#include "Version.h"
#include <QSharedPointer>
#include "VolumeType.h"

// For specifications of the volume header see Common/Volumes.c

namespace GostCrypt
{
namespace Volume {

	typedef quint64 VolumeTime;

	struct VolumeHeaderCreationOptions
	{
        BufferPtr DataKey; // TODO : should be const ?
        QSharedPointer <EncryptionAlgorithm> EA;
        QSharedPointer <VolumeHash> Hash;
        BufferPtr HeaderKey; // TODO : should be const ?
        BufferPtr Salt; // TODO : should be const ?
		quint32 SectorSize;
		quint64 VolumeDataSize;
		quint64 VolumeDataStart;
		VolumeType::Enum Type;
	};

	class VolumeHeader
	{
	public:
        explicit VolumeHeader (quint32 HeaderSize);
		virtual ~VolumeHeader ();

        void Create (BufferPtr &headerBuffer, VolumeHeaderCreationOptions &options);
        bool Decrypt (const BufferPtr &encryptedData, const VolumePassword &password, const VolumeHashList &keyDerivationFunctions, const EncryptionAlgorithmList &encryptionAlgorithms, const EncryptionModeList &encryptionModes);
        void EncryptNew (BufferPtr &newHeaderBuffer, const BufferPtr &newSalt, const BufferPtr &newHeaderKey, QSharedPointer <VolumeHash> newVolumeHash);
		quint64 GetEncryptedAreaStart () const { return EncryptedAreaStart; }
		QSharedPointer <EncryptionAlgorithm> GetEncryptionAlgorithm () const { return EA; }
		VolumeTime GetHeaderCreationTime () const { return HeaderCreationTime; }
		static size_t GetLargestSerializedKeySize ();
        QSharedPointer <VolumeHash> GetVolumeHash () const { return volumeHash; }
		size_t GetSectorSize () const { return SectorSize; }
		static quint32 GetSaltSize () { return SaltSize; }
		quint64 GetVolumeDataSize () const { return VolumeDataSize; }
		VolumeTime GetVolumeCreationTime () const { return VolumeCreationTime; }

	protected:
        bool Deserialize (const BufferPtr &header, QSharedPointer <EncryptionAlgorithm> &ea, QSharedPointer <EncryptionMode> &mode);
        template <typename T> static T DeserializeEntry (const BufferPtr &header, size_t &offset);
        template <typename T> static T DeserializeEntryAt (const BufferPtr &header, const size_t &offset);
		void Init ();
        void Serialize (BufferPtr &header) const;
        template <typename T> static void SerializeEntry (const T &entry, BufferPtr &header, size_t &offset);

		quint32 HeaderSize;

		static const quint16 CurrentHeaderVersion = VOLUME_HEADER_VERSION;
		static const quint16 CurrentRequiredMinProgramVersion = GST_VOLUME_MIN_REQUIRED_PROGRAM_VERSION;
		static const quint16 MinAllowedHeaderVersion = 1;

		static const int SaltOffset = 0;
		static const quint32 SaltSize = 64;

		static const int EncryptedHeaderDataOffset = SaltOffset + SaltSize;
		quint32 EncryptedHeaderDataSize;

		static const quint32 LegacyEncryptionModeKeyAreaSize = 32;
		static const int DataKeyAreaMaxSize = 256;
		static const quint32 DataAreaKeyOffset = DataKeyAreaMaxSize - EncryptedHeaderDataOffset;

		QSharedPointer <EncryptionAlgorithm> EA;
        QSharedPointer <VolumeHash> volumeHash;

		quint16 HeaderVersion;
		quint16 RequiredMinProgramVersion;
		quint32 VolumeKeyAreaCrc32;

		VolumeTime VolumeCreationTime;
		VolumeTime HeaderCreationTime;

		VolumeType::Enum mVolumeType;
		quint64 HiddenVolumeDataSize;
		quint64 VolumeDataSize;
		quint64 EncryptedAreaStart;
		quint64 EncryptedAreaLength;
		quint32 Flags;
		quint32 SectorSize;

		SecureBuffer DataAreaKey;

	private:
		VolumeHeader (const VolumeHeader &);
		VolumeHeader &operator= (const VolumeHeader &);
	};
}
}
#endif // GST_HEADER_Volume_VolumeHeader
