/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "VolumePassword.h"

namespace GostCrypt
{
namespace Volume {

	VolumePassword::VolumePassword () : PasswordSize (0), Unportable (false)
	{
		AllocateBuffer ();
	}

	VolumePassword::VolumePassword (const char *password, size_t size)
	{
		Set ((const quint8 *) password, size);
	}

	VolumePassword::VolumePassword (const quint8 *password, size_t size)
	{
		Set (password, size);
	}

	VolumePassword::VolumePassword (const wchar_t *password, size_t charCount)
	{
		Set (password, charCount);
	}

    /*
        VolumePassword::VolumePassword (const std::wstring &password)
	{
		Set (password.c_str(), password.size());
        }//*/

	VolumePassword::~VolumePassword ()
	{
	}

	void VolumePassword::AllocateBuffer ()
	{
		if (!PasswordBuffer.IsAllocated ())
			PasswordBuffer.Allocate (MaxSize);
	}

	bool VolumePassword::IsPortable () const
	{
		for (size_t i = 0; i < PasswordSize; i++)
		{
			if (PasswordBuffer[i] >= 0x7f || PasswordBuffer[i] < 0x20)
				return false;
		}
		return true;
	}

	void VolumePassword::Set (const quint8 *password, size_t size)
	{
		AllocateBuffer ();

		if (size > MaxSize)
            throw;// (SRC_POS);

		PasswordBuffer.CopyFrom (ConstBufferPtr (password, size));
		PasswordSize = size;

		Unportable = !IsPortable();
	}

	void VolumePassword::Set (const wchar_t *password, size_t charCount)
	{
		if (charCount > MaxSize)
            throw;// (SRC_POS);

		union Conv
		{
			quint8 b[sizeof (wchar_t)];
			wchar_t c;
		};

		Conv conv;
		conv.c = L'A';

		int lsbPos = -1;
		for (size_t i = 0; i < sizeof (conv.b); ++i)
		{
			if (conv.b[i] == L'A')
			{
				lsbPos = i;
				break;
			}
		}

		if (lsbPos == -1)
            throw;// (SRC_POS);

		bool unportable = false;
		quint8 passwordBuf[MaxSize];
		for (size_t i = 0; i < charCount; ++i)
		{
			conv.c = password[i];
			passwordBuf[i] = conv.b[lsbPos];
			for (int j = 0; j < (int) sizeof (wchar_t); ++j)
			{
				if (j != lsbPos && conv.b[j] != 0)
					unportable = true;
			}
		}

		Set (passwordBuf, charCount);

		if (unportable)
			Unportable = true;
	}

	void VolumePassword::Set (const ConstBufferPtr &password)
	{
		Set (password, password.Size());
	}

	void VolumePassword::Set (const VolumePassword &password)
	{
		Set (password.DataPtr(), password.Size());
	}

}
}
