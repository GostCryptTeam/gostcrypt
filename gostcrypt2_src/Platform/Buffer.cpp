/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Buffer.h"
#include "Volume/VolumeException.h"
#include <sys/mman.h>

namespace GostCrypt
{
    // BufferPtr

    bool BufferPtr::IsDataEqual (const BufferPtr &otherbufferptr) const
    {
        if (DataSize != otherbufferptr.Size())
            return false;
        return memcmp(DataPtrReadonly, otherbufferptr.Get(), DataSize) == 0;
    }

    void BufferPtr::CopyFrom (const BufferPtr &bufferPtr)
    {
        if (bufferPtr.Size() > DataSize)
            throw IncorrectParameterException("bufferPtr.size > DataSize");
        if(!DataPtrMutable)
            throw DataNotMutableException();
        memcpy (DataPtrMutable, bufferPtr.Get(), bufferPtr.Size());
    }

    BufferPtr BufferPtr::GetRange (size_t offset, size_t size)
    {
        if (offset + size > DataSize)
            throw IncorrectParameterException("offset+size > DataSize");
        if(!DataPtrMutable)
            throw DataNotMutableException();
        return BufferPtr (DataPtrMutable + offset, size);
    }

    const BufferPtr BufferPtr::GetRange (size_t offset, size_t size) const
    {
        if (offset + size > DataSize)
            throw IncorrectParameterException("offset+size > DataSize");

        return BufferPtr (DataPtrReadonly + offset, size);
    }

    // Buffer

    Buffer::Buffer (size_t size)
    {
		Allocate (size);
	}

    Buffer::~Buffer ()
    {
        if (Data.Get() != nullptr)
			Free ();
	}

	void Buffer::Allocate (size_t size)
	{
        if (size < 1)
            throw IncorrectParameterException("size is null");

        Usersize = size;

        if (size <= Data.Size())
            return; // everything is fine and alloced already

        if(Data.Size() != 0)
            Free();

        Usersize = size;

		try
		{
            Data.Set(static_cast<quint8 *> (malloc(size)), size); // TODO : calloc ?
            if (!Data.Get())
                throw FailedMemoryAllocationException();
		}
		catch (...)
		{
            Data.Set(nullptr, 0);
            Usersize = 0;
            throw; //rethrow
		}
	}

    void Buffer::CopyFrom (const BufferPtr &bufferPtr)
	{
		if (!IsAllocated ())
			Allocate (bufferPtr.Size());
        else if (bufferPtr.Size() > Usersize)
            throw IncorrectParameterException("bufferPtr.size > DataSize")
        memcpy(Data.Get(), bufferPtr.Get(), bufferPtr.Size());
	}

	void Buffer::Erase ()
	{
        if (Data.Size() > 0)
            memset(Data.Get(), 0, Data.Size()); // erase everything
	}

	void Buffer::Free ()
	{
        if (Data.Get() == nullptr)
            throw BufferAlreadyFreedException();
        free(Data.Get());
        Data.Set(nullptr, 0);
        Usersize = 0;
	}

    BufferPtr Buffer::GetRange (size_t offset, size_t size)
	{
        if (offset + size > Usersize)
            throw IncorrectParameterException("offset+size > Usersize");
        return Data.GetRange(offset, size);
	}

    const BufferPtr Buffer::GetRange (size_t offset, size_t size) const
    {
        if (offset + size > Usersize)
            throw IncorrectParameterException("offset+size > Usersize");
        return Data.GetRange(offset, size);
    }

    // SecureBuffer

	SecureBuffer::~SecureBuffer ()
	{
        if (Data.Get() != nullptr)
			Free ();
	}

    void SecureBuffer::Erase ()
    {
        #ifdef GST_NO_BURN_OPTIMISATION
        volatile size_t number_to_erase8 = Data.Size();
        volatile quint8 *burnm8 = (volatile quint8 *)(Data.Get());
        while(number_to_erase8--) *burnm8++ = 0;
        #else
        #ifndef GST_NO_COMPILER_INT64
        volatile size_t number_to_erase64 = Data.Size() >> 3;
        volatile size_t number_to_erase8 = Data.Size() % 8;
        volatile quint64 *burnm64 = (volatile quint64 *)(Data.Get());
        volatile quint8 *burnm8;
        while(number_to_erase64--) *burnm64++ = 0;
        burnm8 = (volatile quint8 *) burnm64;
        while(number_to_erase8--) *burnm8++ = 0;
        #else
        volatile size_t number_to_erase32 = Data.Size() >> 2;
        volatile size_t number_to_erase8 = Data.Size() % 4;
        volatile quint32 *burnm32 = (volatile quint32 *)(Data.Get());
        volatile quint8 *burnm8;
        while(number_to_erase32--) *burnm32++ = 0;
        burnm8 = (volatile quint8 *) burnm32;
        while(number_to_erase8--) *burnm8++ = 0;
        #endif
        #endif
    }

	void SecureBuffer::Allocate (size_t size)
	{
		Buffer::Allocate (size);
        mlock(Data.Get(), Data.Size()); // locking mem to RAM
	}

	void SecureBuffer::Free ()
	{
        if (Data.Get() == nullptr)
            throw BufferAlreadyFreedException();

		Erase ();
        munlock(Data.Get(), Data.Size()); // do not forget to unlock memory
		Buffer::Free ();
	}
}
