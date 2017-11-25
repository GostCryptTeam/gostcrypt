/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Encryption_Password
#define GST_HEADER_Encryption_Password

#include "Platform/Platform.h"

namespace GostCrypt
{
namespace Volume {

    class VolumePassword
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

		static const size_t MaxSize = 64;
		static const size_t WarningSizeThreshold = 12;

	protected:
		void AllocateBuffer ();
		bool IsPortable () const;

		SecureBuffer PasswordBuffer;

		size_t PasswordSize;
		bool Unportable;
	};

}
}
#endif // GST_HEADER_Encryption_Password
