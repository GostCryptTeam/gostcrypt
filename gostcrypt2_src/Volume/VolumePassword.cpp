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

    bool VolumePassword::operator==(const VolumePassword &other) const
    {
        const BufferPtr data1(DataPtr(), Size());
        const BufferPtr data2(other.DataPtr(), other.Size());
        return data1.IsDataEqual(data2);
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
            throw IncorrectParameterException("Password size too big");

        PasswordBuffer.CopyFrom (BufferPtr (password, size));
		PasswordSize = size;
	}

    void VolumePassword::Set (const BufferPtr &password)
	{
        Set (password.Get(), password.Size());
	}
}
}
