/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Ciphers
#define GST_HEADER_Encryption_Ciphers

#include "Platform/Platform.h"


namespace GostCrypt
{
namespace Volume {

    class CipherAlgorithm;
    typedef std::vector < QSharedPointer <CipherAlgorithm> > CipherAlgorithmList;

    class CipherAlgorithm
	{
	public:
        virtual ~CipherAlgorithm ();

		virtual void DecryptBlock (quint8 *data) const;
		virtual void DecryptBlocks (quint8 *data, size_t blockCount) const;
		static void EnableHwSupport (bool enable) { HwSupportEnabled = enable; }
		virtual void EncryptBlock (quint8 *data) const;
		virtual void EncryptBlocks (quint8 *data, size_t blockCount) const;
        static CipherAlgorithmList GetAvailableCiphers ();
		virtual size_t GetBlockSize () const = 0;
		virtual const SecureBuffer &GetKey () const { return Key; }
		virtual size_t GetKeySize () const = 0;
        virtual std::wstring GetName () const = 0;
        virtual std::wstring GetDescription () const = 0;
        virtual QSharedPointer <CipherAlgorithm> GetNew () const = 0;
		virtual bool IsHwSupportAvailable () const { return false; }
		static bool IsHwSupportEnabled () { return HwSupportEnabled; }
		virtual void SetKey (const ConstBufferPtr &key);

		virtual void StoreCipherKey ();
		virtual void RestoreCipherKey ();
		virtual bool IsKeySwapped ();
		virtual void XorCipherKey (quint8 *ks, quint8 *data, int len) const = 0;
		virtual void EncryptWithKS (quint8 *data, quint8 *ks) const = 0;
		virtual void DecryptWithKS (quint8 *data, quint8 *ks) const = 0;
		virtual void CopyCipherKey (quint8 *ks) const = 0;

		static const int MaxBlockSize = 16;
		virtual size_t GetScheduledKeySize () const = 0;

	protected:
        CipherAlgorithm ();

		virtual void Decrypt (quint8 *data) const = 0;
		virtual void Encrypt (quint8 *data) const = 0;
		virtual void SetCipherKey (const quint8 *key) = 0;

		static bool HwSupportEnabled;
		bool Initialized;
		SecureBuffer Key;
		SecureBuffer ScheduledKey;
		SecureBuffer SwapScheduledKey;
		bool KeySwapped;

	private:
        CipherAlgorithm (const CipherAlgorithm &);
        CipherAlgorithm &operator= (const CipherAlgorithm &);
	};

#define GST_CIPHER(NAME, BLOCK_SIZE, KEY_SIZE, FANCY_NAME, DESCRIPTION) \
    class CipherAlgorithm##NAME : public CipherAlgorithm \
	{ \
	public: \
        CipherAlgorithm##NAME () { } \
        virtual ~CipherAlgorithm##NAME () { } \
\
		virtual size_t GetBlockSize () const { return BLOCK_SIZE; }; \
		virtual size_t GetKeySize () const { return KEY_SIZE; }; \
                virtual std::wstring GetName () const { return L##FANCY_NAME; }; \
                virtual std::wstring GetDescription () const { return L##DESCRIPTION; }; \
                virtual QSharedPointer <CipherAlgorithm> GetNew () const { return QSharedPointer <CipherAlgorithm> (new CipherAlgorithm##NAME()); } \
\
                virtual void XorCipherKey (quint8 *ks, quint8 *data, int len) const; \
                virtual void EncryptWithKS (quint8 *data, quint8 *ks) const; \
                virtual void DecryptWithKS (quint8 *data, quint8 *ks) const; \
                virtual void CopyCipherKey (quint8 *ks) const; \
                virtual size_t GetScheduledKeySize () const; \
\
	protected: \
        virtual void Decrypt (quint8 *data) const; \
                virtual void Encrypt (quint8 *data) const; \
        virtual void SetCipherKey (const quint8 *key); \
\
	private: \
        CipherAlgorithm##NAME (const CipherAlgorithm##NAME &); \
        CipherAlgorithm##NAME &operator= (const CipherAlgorithm##NAME &); \
        };

GST_CIPHER(GOST, 8, 32, "GOST 28147-89",
           "The GOST 28147-89 block cipher is part of the Russian cryptographic"
           "standard algorithms. It is a symmetric key block cipher based on a"
           "balanced Feistel network.<br><font color='719c24'><a href='#'>More information on GOST 28147-89</a></font>")

GST_CIPHER(GRASSHOPPER, 8, 32, "Gost Grasshopper", "No description for Gost Grasshopper.")

}
}
#endif // GST_HEADER_Encryption_Ciphers
