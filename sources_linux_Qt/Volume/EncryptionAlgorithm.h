/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_EncryptionAlgorithm
#define GST_HEADER_Encryption_EncryptionAlgorithm

#include "Platform/Platform.h"
#include "Cipher.h"
#include "EncryptionMode.h"

namespace GostCrypt
{
	class EncryptionAlgorithm;
    typedef list < shared_ptr <GostCrypt::EncryptionAlgorithm> > EncryptionAlgorithmList;

	class EncryptionAlgorithm
	{
	public:
		virtual ~EncryptionAlgorithm ();

		virtual void Decrypt (byte *data, uint64 length) const;
		virtual void Decrypt (const BufferPtr &data) const;
		virtual void DecryptSectors (byte *data, uint64 sectorIndex, uint64 sectorCount, size_t sectorSize) const;
		virtual void Encrypt (byte *data, uint64 length) const;
		virtual void Encrypt (const BufferPtr &data) const;
		virtual void EncryptSectors (byte *data, uint64 sectorIndex, uint64 sectorCount, size_t sectorSize) const;
		static EncryptionAlgorithmList GetAvailableAlgorithms ();
		virtual const CipherList &GetCiphers () const { return Ciphers; }
		virtual shared_ptr <EncryptionAlgorithm> GetNew () const = 0;
		virtual size_t GetMaxBlockSize () const;
		virtual size_t GetMinBlockSize () const;
		static size_t GetLargestKeySize (const EncryptionAlgorithmList &algorithms);
		virtual size_t GetKeySize () const;
		virtual shared_ptr <EncryptionMode> GetMode () const;
		virtual wstring GetName () const;
		bool IsDeprecated () const { return Deprecated; }
		virtual bool IsModeSupported (const EncryptionMode &mode) const;
		virtual bool IsModeSupported (const shared_ptr <EncryptionMode> mode) const;
		virtual void SetKey (const ConstBufferPtr &key);
		virtual void SetMode (shared_ptr <EncryptionMode> mode);

	protected:
		EncryptionAlgorithm ();

		void ValidateState () const;

		CipherList Ciphers;
		bool Deprecated;
		shared_ptr <EncryptionMode> Mode;
		EncryptionModeList SupportedModes;

	private:
		EncryptionAlgorithm (const EncryptionAlgorithm &);
		EncryptionAlgorithm &operator= (const EncryptionAlgorithm &);
	};

#define GST_ENCRYPTION_ALGORITHM(NAME) \
	class NAME : public EncryptionAlgorithm \
	{ \
	public: \
		NAME (); \
		virtual ~NAME () { } \
\
		virtual shared_ptr <EncryptionAlgorithm> GetNew () const { return shared_ptr <EncryptionAlgorithm> (new NAME()); } \
\
	private: \
		NAME (const NAME &); \
		NAME &operator= (const NAME &); \
	}
	
	GST_ENCRYPTION_ALGORITHM (GOST);
	GST_ENCRYPTION_ALGORITHM (GRASSHOPPER);

#undef GST_ENCRYPTION_ALGORITHM
}

#endif // GST_HEADER_Encryption_EncryptionAlgorithm
