/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_VolumeHeader
#define GST_HEADER_Volume_VolumeHeader

#include "Common/Gstdefs.h"
#include "Common/Volumes.h"
#include "Platform/Platform.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Volume/EncryptionMode.h"
#include "Volume/Keyfile.h"
#include "Volume/VolumePassword.h"
#include "Volume/Pkcs5Kdf.h"
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
		ConstBufferPtr DataKey;
        QSharedPointer <EncryptionAlgorithm> EA;
        QSharedPointer <Pkcs5Kdf> Kdf;
		ConstBufferPtr HeaderKey;
		ConstBufferPtr Salt;
		quint32 SectorSize;
		quint64 VolumeDataSize;
		quint64 VolumeDataStart;
		VolumeType::Enum Type;
	};

	class VolumeHeader
	{
	public:
		VolumeHeader (quint32 HeaderSize);
		virtual ~VolumeHeader ();

		void Create (const BufferPtr &headerBuffer, VolumeHeaderCreationOptions &options);
		bool Decrypt (const ConstBufferPtr &encryptedData, const VolumePassword &password, const Pkcs5KdfList &keyDerivationFunctions, const EncryptionAlgorithmList &encryptionAlgorithms, const EncryptionModeList &encryptionModes);
		void EncryptNew (const BufferPtr &newHeaderBuffer, const ConstBufferPtr &newSalt, const ConstBufferPtr &newHeaderKey, QSharedPointer <Pkcs5Kdf> newPkcs5Kdf);
		quint64 GetEncryptedAreaStart () const { return EncryptedAreaStart; }
		quint64 GetEncryptedAreaLength () const { return EncryptedAreaLength; }
		QSharedPointer <EncryptionAlgorithm> GetEncryptionAlgorithm () const { return EA; }
		quint32 GetFlags () const { return Flags; }
		VolumeTime GetHeaderCreationTime () const { return HeaderCreationTime; }
		quint64 GetHiddenVolumeDataSize () const { return HiddenVolumeDataSize; }
		static size_t GetLargestSerializedKeySize ();
		QSharedPointer <Pkcs5Kdf> GetPkcs5Kdf () const { return Pkcs5; }
		quint16 GetRequiredMinProgramVersion () const { return RequiredMinProgramVersion; }
		size_t GetSectorSize () const { return SectorSize; }
		static quint32 GetSaltSize () { return SaltSize; }
		quint64 GetVolumeDataSize () const { return VolumeDataSize; }
		VolumeTime GetVolumeCreationTime () const { return VolumeCreationTime; }
		void SetSize (quint32 headerSize);
		quint16 GetHeaderVersion () const { return HeaderVersion; }

	protected:
		bool Deserialize (const ConstBufferPtr &header, QSharedPointer <EncryptionAlgorithm> &ea, QSharedPointer <EncryptionMode> &mode);
		template <typename T> T DeserializeEntry (const ConstBufferPtr &header, size_t &offset) const;
		template <typename T> T DeserializeEntryAt (const ConstBufferPtr &header, const size_t &offset) const;
		void Init ();
		void Serialize (const BufferPtr &header) const;
		template <typename T> void SerializeEntry (const T &entry, const BufferPtr &header, size_t &offset) const;

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
		QSharedPointer <Pkcs5Kdf> Pkcs5;

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
