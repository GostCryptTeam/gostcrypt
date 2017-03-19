/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Hash
#define GST_HEADER_Encryption_Hash

#include "Platform/Platform.h"

namespace GostCrypt
{
	class Hash;
	typedef list < shared_ptr <Hash> > HashList;

	class Hash
	{
	public:
		Hash () : Deprecated (false) { }
		virtual ~Hash () { }

		static HashList GetAvailableAlgorithms ();
		virtual void GetDigest (const BufferPtr &buffer) = 0;
		virtual size_t GetBlockSize () const = 0;
		virtual size_t GetDigestSize () const = 0;
		virtual wstring GetName () const = 0;
		virtual shared_ptr <Hash> GetNew () const = 0;
		virtual void Init () = 0;
		bool IsDeprecated () const { return Deprecated; }
		virtual void ProcessData (const ConstBufferPtr &data) = 0;
		virtual void ValidateDataParameters (const ConstBufferPtr &data) const;
		virtual void ValidateDigestParameters (const BufferPtr &buffer) const;

	protected:
		SecureBuffer Context;
		bool Deprecated;

	private:
		Hash (const Hash &);
		Hash &operator= (const Hash &);
	};

	// Whirlpool
	class Whirlpool : public Hash
	{
	public:
		Whirlpool ();
		virtual ~Whirlpool () { }

		virtual void GetDigest (const BufferPtr &buffer);
		virtual size_t GetBlockSize () const { return 64; }
		virtual size_t GetDigestSize () const { return 512 / 8; }
		virtual wstring GetName () const { return L"Whirlpool"; }
		virtual shared_ptr <Hash> GetNew () const { return shared_ptr <Hash> (new Whirlpool); }
		virtual void Init ();
		virtual void ProcessData (const ConstBufferPtr &data);

	protected:

	private:
		Whirlpool (const Whirlpool &);
		Whirlpool &operator= (const Whirlpool &);
	};
	
	// Stribog
	class Stribog : public Hash
	{
	public:
		Stribog ();
		virtual ~Stribog () { }
		
		virtual void GetDigest (const BufferPtr &buffer);
		virtual size_t GetBlockSize () const { return 64; }
		virtual size_t GetDigestSize () const { return 64; }
		virtual wstring GetName () const { return L"GOST R 34.11-2012"; }
		virtual shared_ptr <Hash> GetNew () const { return shared_ptr <Hash> (new Stribog); }
		virtual void Init ();
		virtual void ProcessData (const ConstBufferPtr &data);

	protected:

	private:
		Stribog (const Stribog &);
		Stribog &operator= (const Stribog &);
	};
	
	// GOST R 34.11-94
	class GostHash : public Hash
	{
	public:
		GostHash ();
		virtual ~GostHash () { }
		
		virtual void GetDigest (const BufferPtr &buffer);
		virtual size_t GetBlockSize () const { return 32; }
		virtual size_t GetDigestSize () const { return 32; }
		virtual wstring GetName () const { return L"GOST R 34.11-94"; }
		virtual shared_ptr <Hash> GetNew () const { return shared_ptr <Hash> (new GostHash); }
		virtual void Init ();
		virtual void ProcessData (const ConstBufferPtr &data);

	protected:

	private:
		GostHash (const GostHash &);
		GostHash &operator= (const GostHash &);
	};
}

#endif // GST_HEADER_Encryption_Hash
