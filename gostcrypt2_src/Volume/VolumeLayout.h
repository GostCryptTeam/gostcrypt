/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Volume_VolumeLayout
#define GST_HEADER_Volume_VolumeLayout

#include "Platform/Platform.h"
#include "Volume/EncryptionAlgorithm.h"
#include "Volume/EncryptionMode.h"
#include "Volume/Pkcs5Kdf.h"
#include "VolumeHeader.h"

namespace GostCrypt
{
namespace Volume {

	class VolumeLayout;
	typedef list < QSharedPointer <VolumeLayout> > VolumeLayoutList;

	class VolumeLayout
	{
	public:
		virtual ~VolumeLayout ();

		static VolumeLayoutList GetAvailableLayouts (VolumeType::Enum type = VolumeType::Unknown);
		virtual int GetBackupHeaderOffset () const { return BackupHeaderOffset; } // Positive value: offset from the start of host, negative: offset from the end
		virtual uint64 GetDataOffset (uint64 volumeHostSize) const = 0;
		virtual uint64 GetDataSize (uint64 volumeHostSize) const = 0;
        virtual QSharedPointer <VolumeHeader> GetHeader ();
		virtual int GetHeaderOffset () const { return HeaderOffset; } // Positive value: offset from the start of host, negative: offset from the end
		virtual uint32 GetHeaderSize () const { return HeaderSize; }
		virtual uint64 GetMaxDataSize (uint64 volumeSize) const = 0;
		virtual EncryptionAlgorithmList GetSupportedEncryptionAlgorithms () const { return SupportedEncryptionAlgorithms; }
		virtual Pkcs5KdfList GetSupportedKeyDerivationFunctions () const { return Pkcs5Kdf::GetAvailableAlgorithms(); }
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
		uint32 HeaderSize;
		VolumeType::Enum Type;

        QSharedPointer <VolumeHeader> Header;

	private:
		VolumeLayout (const VolumeLayout &);
		VolumeLayout &operator= (const VolumeLayout &);
	};


	class VolumeLayoutV1Normal : public VolumeLayout
	{
	public:
		VolumeLayoutV1Normal ();
		virtual ~VolumeLayoutV1Normal () { }

		virtual int GetBackupHeaderOffset () const { throw NotApplicable (SRC_POS); }
		virtual uint64 GetDataOffset (uint64 volumeHostSize) const;
		virtual uint64 GetDataSize (uint64 volumeHostSize) const;
                virtual uint64 GetMaxDataSize (uint64 volumeSize) const { (void)volumeSize; throw NotApplicable (SRC_POS); }
		virtual bool HasBackupHeader () const { return false; }

	private:
		VolumeLayoutV1Normal (const VolumeLayoutV1Normal &);
		VolumeLayoutV1Normal &operator= (const VolumeLayoutV1Normal &);
	};


	class VolumeLayoutV1Hidden : public VolumeLayout
	{
	public:
		VolumeLayoutV1Hidden ();
		virtual ~VolumeLayoutV1Hidden () { }

		virtual int GetBackupHeaderOffset () const { throw NotApplicable (SRC_POS); }
		virtual uint64 GetDataOffset (uint64 volumeHostSize) const;
		virtual uint64 GetDataSize (uint64 volumeHostSize) const;
                virtual uint64 GetMaxDataSize (uint64 volumeSize) const { (void)volumeSize; throw NotApplicable (SRC_POS); }
		virtual bool HasBackupHeader () const { return false; }

	private:
		VolumeLayoutV1Hidden (const VolumeLayoutV1Hidden &);
		VolumeLayoutV1Hidden &operator= (const VolumeLayoutV1Hidden &);
	};


	class VolumeLayoutV2Normal : public VolumeLayout
	{
	public:
		VolumeLayoutV2Normal ();
		virtual ~VolumeLayoutV2Normal () { }

		virtual uint64 GetDataOffset (uint64 volumeHostSize) const;
		virtual uint64 GetDataSize (uint64 volumeHostSize) const;
		virtual uint64 GetMaxDataSize (uint64 volumeSize) const;
		virtual bool HasBackupHeader () const { return true; }

	private:
		VolumeLayoutV2Normal (const VolumeLayoutV2Normal &);
		VolumeLayoutV2Normal &operator= (const VolumeLayoutV2Normal &);
	};


	class VolumeLayoutV2Hidden : public VolumeLayout
	{
	public:
		VolumeLayoutV2Hidden ();
		virtual ~VolumeLayoutV2Hidden () { }

		virtual uint64 GetDataOffset (uint64 volumeHostSize) const;
		virtual uint64 GetDataSize (uint64 volumeHostSize) const;
		virtual uint64 GetMaxDataSize (uint64 volumeSize) const;
		virtual bool HasBackupHeader () const { return true; }

	private:
		VolumeLayoutV2Hidden (const VolumeLayoutV2Hidden &);
		VolumeLayoutV2Hidden &operator= (const VolumeLayoutV2Hidden &);
	};
}
}

#endif // GST_HEADER_Volume_VolumeLayout
