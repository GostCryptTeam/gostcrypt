/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_VolumeLayout
#define GST_HEADER_Volume_VolumeLayout

#include "Volume/EncryptionAlgorithm.h"
#include "Volume/EncryptionMode.h"
#include "VolumeHeader.h"

namespace GostCrypt {
namespace Volume {

	class VolumeLayout;
    typedef QList < QSharedPointer <VolumeLayout> > VolumeLayoutList;

	class VolumeLayout
	{
	public:
		virtual ~VolumeLayout ();

		static VolumeLayoutList GetAvailableLayouts (VolumeType::Enum type = VolumeType::Unknown);
		virtual int GetBackupHeaderOffset () const { return BackupHeaderOffset; } // Positive value: offset from the start of host, negative: offset from the end
		virtual quint64 GetDataOffset (quint64 volumeHostSize) const = 0;
		virtual quint64 GetDataSize (quint64 volumeHostSize) const = 0;
        virtual QSharedPointer <VolumeHeader> GetHeader ();
		virtual int GetHeaderOffset () const { return HeaderOffset; } // Positive value: offset from the start of host, negative: offset from the end
		virtual quint32 GetHeaderSize () const { return HeaderSize; }
		virtual quint64 GetMaxDataSize (quint64 volumeSize) const = 0;
		virtual EncryptionAlgorithmList GetSupportedEncryptionAlgorithms () const { return SupportedEncryptionAlgorithms; }
        virtual VolumeHashList GetSupportedKeyDerivationFunctions () const { return VolumeHash::GetAvailableAlgorithms(); }
		virtual EncryptionModeList GetSupportedEncryptionModes () const { return SupportedEncryptionModes; }
		virtual VolumeType::Enum GetType () const { return Type; }
		virtual bool HasBackupHeader () const = 0;
        virtual void SetHeader (QSharedPointer <VolumeHeader> header) { Header = header; }

	protected:
		VolumeLayout ();

		EncryptionAlgorithmList SupportedEncryptionAlgorithms;
		EncryptionModeList SupportedEncryptionModes;

		int BackupHeaderOffset;
		int HeaderOffset;
		quint32 HeaderSize;
		VolumeType::Enum Type;

        QSharedPointer <VolumeHeader> Header;

	private:
		VolumeLayout (const VolumeLayout &);
		VolumeLayout &operator= (const VolumeLayout &);
	};

}
}

#endif // GST_HEADER_Volume_VolumeLayout
