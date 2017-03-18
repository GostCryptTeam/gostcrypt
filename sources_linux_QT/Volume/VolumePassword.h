/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Password
#define GST_HEADER_Encryption_Password

#include "Platform/Platform.h"
#include "Platform/Serializable.h"

namespace GostCrypt
{
	class VolumePassword : public Serializable
	{
	public:
		VolumePassword ();
		VolumePassword (const byte *password, size_t size);   
		VolumePassword (const char *password, size_t size);
		VolumePassword (const wchar_t *password, size_t charCount);
		VolumePassword (const wstring &password);
		VolumePassword (const VolumePassword &password) { Set (password); }
		virtual ~VolumePassword ();

		bool operator== (const VolumePassword &other) const { return ConstBufferPtr (DataPtr(), Size()).IsDataEqual (ConstBufferPtr (other.DataPtr(), other.Size())); }
		bool operator!= (const VolumePassword &other) const { return !(*this == other); }
		VolumePassword &operator= (const VolumePassword &password) { Set (password); return *this; }

		operator BufferPtr () const { return BufferPtr (PasswordBuffer); }

		void CheckPortability () const;
		byte *DataPtr () const { return PasswordBuffer; }
		bool IsEmpty () const { return PasswordSize == 0; }
		size_t Size () const { return PasswordSize; }
		void Set (const byte *password, size_t size);
		void Set (const wchar_t *password, size_t charCount);
		void Set (const ConstBufferPtr &password);
		void Set (const VolumePassword &password);

		GST_SERIALIZABLE (VolumePassword);

		static const size_t MaxSize = 64;
		static const size_t WarningSizeThreshold = 12;

	protected:
		void AllocateBuffer ();
		bool IsPortable () const;

		SecureBuffer PasswordBuffer;

		size_t PasswordSize;
		bool Unportable;
	};

	struct PasswordException : public Exception
	{
	protected:
		PasswordException () { }
		PasswordException (const string &message) : Exception (message) { }
		PasswordException (const string &message, const wstring &subject) : Exception (message, subject) { }
	};

	GST_EXCEPTION_DECL (PasswordIncorrect, PasswordException);
	GST_EXCEPTION_DECL (PasswordKeyfilesIncorrect, PasswordIncorrect);
	GST_EXCEPTION_DECL (PasswordOrKeyboardLayoutIncorrect, PasswordException);
	GST_EXCEPTION_DECL (PasswordOrMountOptionsIncorrect, PasswordException);
	GST_EXCEPTION_DECL (ProtectionPasswordIncorrect, PasswordIncorrect);
	GST_EXCEPTION_DECL (ProtectionPasswordKeyfilesIncorrect, PasswordIncorrect);

#define GST_EXCEPTION(NAME) GST_EXCEPTION_DECL(NAME,PasswordException)

#undef GST_EXCEPTION_SET
#define GST_EXCEPTION_SET \
	GST_EXCEPTION_NODECL (PasswordIncorrect); \
	GST_EXCEPTION_NODECL (PasswordKeyfilesIncorrect); \
	GST_EXCEPTION_NODECL (PasswordOrKeyboardLayoutIncorrect); \
	GST_EXCEPTION_NODECL (PasswordOrMountOptionsIncorrect); \
	GST_EXCEPTION_NODECL (ProtectionPasswordIncorrect); \
	GST_EXCEPTION_NODECL (ProtectionPasswordKeyfilesIncorrect); \
	GST_EXCEPTION (PasswordEmpty); \
	GST_EXCEPTION (PasswordTooLong); \
	GST_EXCEPTION (UnportablePassword);

	GST_EXCEPTION_SET;

#undef GST_EXCEPTION
}

#endif // GST_HEADER_Encryption_Password
