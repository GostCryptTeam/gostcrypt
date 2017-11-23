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
    typedef vector < shared_ptr <CipherAlgorithm> > CipherList;

    class CipherAlgorithm
	{
	public:
        virtual ~CipherAlgorithm ();

		virtual void DecryptBlock (byte *data) const;
		virtual void DecryptBlocks (byte *data, size_t blockCount) const;
		static void EnableHwSupport (bool enable) { HwSupportEnabled = enable; }
		virtual void EncryptBlock (byte *data) const;
		virtual void EncryptBlocks (byte *data, size_t blockCount) const;
		static CipherList GetAvailableCiphers ();
		virtual size_t GetBlockSize () const = 0;
		virtual const SecureBuffer &GetKey () const { return Key; }
		virtual size_t GetKeySize () const = 0;
		virtual wstring GetName () const = 0;
                virtual wstring GetDescription () const = 0;
        virtual shared_ptr <CipherAlgorithm> GetNew () const = 0;
		virtual bool IsHwSupportAvailable () const { return false; }
		static bool IsHwSupportEnabled () { return HwSupportEnabled; }
		virtual void SetKey (const ConstBufferPtr &key);

		virtual void StoreCipherKey ();
		virtual void RestoreCipherKey ();
		virtual bool IsKeySwapped ();
		virtual void XorCipherKey (byte *ks, byte *data, int len) const = 0;
		virtual void EncryptWithKS (byte *data, byte *ks) const = 0;
		virtual void DecryptWithKS (byte *data, byte *ks) const = 0;
		virtual void CopyCipherKey (byte *ks) const = 0;

		static const int MaxBlockSize = 16;
		virtual size_t GetScheduledKeySize () const = 0;

	protected:
        CipherAlgorithm ();

		virtual void Decrypt (byte *data) const = 0;
		virtual void Encrypt (byte *data) const = 0;
		virtual void SetCipherKey (const byte *key) = 0;

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

	struct CipherException : public Exception
	{
	protected:
		CipherException () { }
		CipherException (const string &message) : Exception (message) { }
		CipherException (const string &message, const wstring &subject) : Exception (message, subject) { }
	};

#define GST_CIPHER(NAME, BLOCK_SIZE, KEY_SIZE, FANCY_NAME, DESCRIPTION) \
    class GST_JOIN (CipherAlgorithm,NAME) : public CipherAlgorithm \
	{ \
	public: \
        GST_JOIN (CipherAlgorithm,NAME) () { } \
        virtual ~GST_JOIN (CipherAlgorithm,NAME) () { } \
\
		virtual size_t GetBlockSize () const { return BLOCK_SIZE; }; \
		virtual size_t GetKeySize () const { return KEY_SIZE; }; \
                virtual wstring GetName () const { return L##FANCY_NAME; }; \
                virtual wstring GetDescription () const { return L##DESCRIPTION; }; \
                virtual shared_ptr <CipherAlgorithm> GetNew () const { return shared_ptr <CipherAlgorithm> (new GST_JOIN (CipherAlgorithm,NAME)()); } \
\
                virtual void XorCipherKey (byte *ks, byte *data, int len) const; \
                virtual void EncryptWithKS (byte *data, byte *ks) const; \
                virtual void DecryptWithKS (byte *data, byte *ks) const; \
                virtual void CopyCipherKey (byte *ks) const; \
                virtual size_t GetScheduledKeySize () const; \
\
	protected: \
		virtual void Decrypt (byte *data) const; \
                virtual void Encrypt (byte *data) const; \
		virtual void SetCipherKey (const byte *key); \
\
	private: \
        GST_JOIN (CipherAlgorithm,NAME) (const GST_JOIN (CipherAlgorithm,NAME) &); \
        GST_JOIN (CipherAlgorithm,NAME) &operator= (const GST_JOIN (CipherAlgorithm,NAME) &); \
        };

GST_CIPHER(GOST, 8, 32, "GOST 28147-89",
           "The GOST 28147-89 block cipher is part of the Russian cryptographic"
           "standard algorithms. It is a symmetric key block cipher based on a"
           "balanced Feistel network.<br><font color='719c24'><a href='#'>More information on GOST 28147-89</a></font>")

GST_CIPHER(GRASSHOPPER, 8, 32, "Gost Grasshopper", "No description for Gost Grasshopper.")

  /*
class CipherGOST : public Cipher
{
public:
	CipherGOST () { }
	virtual ~CipherGOST () { }

	virtual size_t GetBlockSize () const { return 8; };
	virtual size_t GetKeySize () const { return 32; };
	virtual wstring GetName () const { return L"GOST 28147-89"; };
	virtual shared_ptr <Cipher> GetNew () const { return shared_ptr <Cipher> (new CipherGOST()); }

	virtual void XorCipherKey (byte *ks, byte *data, int len) const;
	virtual void EncryptWithKS (byte *data, byte *ks) const;
	virtual void DecryptWithKS (byte *data, byte *ks) const;
	virtual void CopyCipherKey (byte *ks) const;
	virtual size_t GetScheduledKeySize () const;

protected:
	virtual void Decrypt (byte *data) const;
	virtual void Encrypt (byte *data) const;
	virtual void SetCipherKey (const byte *key);

private:
	CipherGOST (const CipherGOST &);
	CipherGOST &operator= (const CipherGOST &);
};

class CipherGRASSHOPPER : public Cipher
{
public:
	CipherGRASSHOPPER () { }
	virtual ~CipherGRASSHOPPER () { }

	virtual size_t GetBlockSize () const {return 16; };
	virtual size_t GetKeySize () const {return 32; };
	virtual wstring GetName() const {return L"Gost Grasshopper"; };
	virtual shared_ptr <Cipher> GetNew() const {return shared_ptr <Cipher> (new CipherGRASSHOPPER());};

	virtual void XorCipherKey (byte *ks, byte *data, int len) const;
	virtual void EncryptWithKS (byte *data, byte *ks) const;
	virtual void DecryptWithKS (byte *data, byte *ks) const;
	virtual void CopyCipherKey (byte *ks) const;

	virtual size_t GetScheduledKeySize () const;

protected:
	virtual void Decrypt (byte *data) const;
	virtual void Encrypt (byte *data) const;
	virtual void SetCipherKey (const byte *key);

private:
	CipherGRASSHOPPER (const CipherGRASSHOPPER &);
	CipherGRASSHOPPER &operator= (const CipherGRASSHOPPER &);

};*/

#define GST_EXCEPTION(NAME) GST_EXCEPTION_DECL(NAME,CipherException)

#undef GST_EXCEPTION_SET
#define GST_EXCEPTION_SET \
	GST_EXCEPTION (CipherInitError); \
        GST_EXCEPTION (WeakKeyDetected)

	GST_EXCEPTION_SET;

#undef GST_EXCEPTION

}
}
#endif // GST_HEADER_Encryption_Ciphers
