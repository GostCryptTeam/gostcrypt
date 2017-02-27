/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Pkcs5
#define GST_HEADER_Encryption_Pkcs5

#include "Platform/Platform.h"
#include "Hash.h"
#include "VolumePassword.h"

namespace GostCrypt
{
	class Pkcs5Kdf;
	typedef list < shared_ptr <Pkcs5Kdf> > Pkcs5KdfList;

	class Pkcs5Kdf
	{
	public:
		virtual ~Pkcs5Kdf ();

		virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt) const;
		virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const = 0;
		static shared_ptr <Pkcs5Kdf> GetAlgorithm (const wstring &name);
		static shared_ptr <Pkcs5Kdf> GetAlgorithm (const Hash &hash);
		static Pkcs5KdfList GetAvailableAlgorithms ();
		virtual shared_ptr <Hash> GetHash () const = 0;
		virtual int GetIterationCount () const = 0;
		virtual wstring GetName () const = 0;
		virtual bool IsDeprecated () const { return GetHash()->IsDeprecated(); }

	protected:
		Pkcs5Kdf ();

		void ValidateParameters (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;

	private:
		Pkcs5Kdf (const Pkcs5Kdf &);
		Pkcs5Kdf &operator= (const Pkcs5Kdf &);
	};

	class Pkcs5HmacWhirlpool : public Pkcs5Kdf
	{
	public:
		Pkcs5HmacWhirlpool () { }
		virtual ~Pkcs5HmacWhirlpool () { }

		virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
		virtual shared_ptr <Hash> GetHash () const { return shared_ptr <Hash> (new Whirlpool); }
		virtual int GetIterationCount () const { return 1000; }
		virtual wstring GetName () const { return L"HMAC-Whirlpool"; }

	private:
		Pkcs5HmacWhirlpool (const Pkcs5HmacWhirlpool &);
		Pkcs5HmacWhirlpool &operator= (const Pkcs5HmacWhirlpool &);
	};
	
	class Pkcs5HmacStribog : public Pkcs5Kdf
	{
	public:
		Pkcs5HmacStribog () { }
		virtual ~Pkcs5HmacStribog () { }

		virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
		virtual shared_ptr <Hash> GetHash () const { return shared_ptr <Hash> (new Stribog); }
		virtual int GetIterationCount () const { return 1000; }
		virtual wstring GetName () const { return L"HMAC-GOST R 34.11-2012"; }

	private:
		Pkcs5HmacStribog (const Pkcs5HmacStribog &);
		Pkcs5HmacStribog &operator= (const Pkcs5HmacStribog &);
	};
	
	class Pkcs5HmacGostHash : public Pkcs5Kdf
	{
	public:
		Pkcs5HmacGostHash () { }
		virtual ~Pkcs5HmacGostHash () { }

		virtual void DeriveKey (const BufferPtr &key, const VolumePassword &password, const ConstBufferPtr &salt, int iterationCount) const;
		virtual shared_ptr <Hash> GetHash () const { return shared_ptr <Hash> (new GostHash); }
		virtual int GetIterationCount () const { return 1000; }
		virtual wstring GetName () const { return L"HMAC-GOST R 34.11-94"; }

	private:
		Pkcs5HmacGostHash (const Pkcs5HmacGostHash &);
		Pkcs5HmacGostHash &operator= (const Pkcs5HmacGostHash &);
	};
}

#endif // GST_HEADER_Encryption_Pkcs5
