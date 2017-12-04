/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include "Core/GostCryptException.h"
#include "VolumePassword.h"

namespace GostCrypt
{
namespace Volume {

    VolumePassword::VolumePassword () : PasswordSize (0)
	{
		AllocateBuffer ();
	}

	VolumePassword::VolumePassword (const char *password, size_t size)
	{
		Set ((const quint8 *) password, size);
	}

    void VolumePassword::AllocateBuffer ()
	{
		if (!PasswordBuffer.IsAllocated ())
			PasswordBuffer.Allocate (MaxSize);
	}

	void VolumePassword::Set (const quint8 *password, size_t size)
	{
		AllocateBuffer ();

		if (size > MaxSize)
            throw IncorrectParameterException("Password size too big");

		PasswordBuffer.CopyFrom (ConstBufferPtr (password, size));
		PasswordSize = size;
	}

	void VolumePassword::Set (const ConstBufferPtr &password)
	{
        Set (password.Get(), password.Size());
	}
}
}
