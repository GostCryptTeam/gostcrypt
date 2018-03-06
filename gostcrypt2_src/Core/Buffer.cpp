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

bool BufferPtr::isDataEqual(const BufferPtr& otherbufferptr) const
{
    if (dataSize != otherbufferptr.size())
    {
        return false;
    }
    return memcmp(dataPtrReadonly, otherbufferptr.get(), dataSize) == 0;
}

void BufferPtr::copyFrom(const BufferPtr& bufferPtr)
{
    if (bufferPtr.size() > dataSize)
    {
        throw InvalidParameterException("bufferPtr.size()", "bufferPtr.size > DataSize")
    }
    if (!dataPtrMutable)
    {
        throw DataNotMutableException();
    }
    memcpy(dataPtrMutable, bufferPtr.get(), bufferPtr.size());
}

BufferPtr BufferPtr::getRange(size_t offset, size_t size)
{
    if (offset + size > dataSize)
    {
        throw InvalidParameterException("offset and size", "offset+size > DataSize");
    }
    if (!dataPtrMutable)
    {
        throw DataNotMutableException();
    }
    return BufferPtr(dataPtrMutable + offset, size);
}

const BufferPtr BufferPtr::getRange(size_t offset, size_t size) const
{
    if (offset + size > dataSize)
    {
        throw InvalidParameterException("offset and size", "offset+size > DataSize");
    }

    return BufferPtr(dataPtrReadonly + offset, size);
}

Buffer::Buffer(size_t size)
{
    allocate(size);
}

Buffer::~Buffer()
{
    if (Data.get() != nullptr)
    {
        freeData();
    }
}

void Buffer::allocate(size_t size)
{
    if (size < 1)
    {
        throw InvalidParameterException("size", "size can not be null");
    }

    Usersize = size;

    if (size <= Data.size())
    {
        return;    // everything is fine and alloced already
    }

    if (Data.size() != 0)
    {
        freeData();
    }

    Usersize = size;

    try
    {
        Data.set(static_cast<quint8*>(malloc(size)), size);   // TODO : calloc ? Why not realloc ?
        if (!Data.get())
        {
            throw FailedMemoryAllocationException();
        }
    }
    catch (...)
    {
        Data.set(nullptr, 0);
        Usersize = 0;
        throw; //rethrow
    }
}

void Buffer::copyFrom(const BufferPtr& bufferPtr)
{
    if (!isAllocated())
    {
        allocate(bufferPtr.size());
    }
    else if (bufferPtr.size() > Usersize)
    {
        throw InvalidParameterException("offset and size", "offset+size > Usersize");
    }
    memcpy(Data.get(), bufferPtr.get(), bufferPtr.size());
}

void Buffer::erase()
{
    if (Data.size() > 0)
    {
        memset(Data.get(), 0, Data.size());    // erase everything
    }
}

void Buffer::freeData()
{
    if (Data.get() == nullptr)
    {
        throw BufferAlreadyFreedException();
    }
    free(Data.get());
    Data.set(nullptr, 0);
    Usersize = 0;
}

BufferPtr Buffer::getRange(size_t offset, size_t size)
{
    if (offset + size > Usersize)
    {
        throw InvalidParameterException("offset and size", "offset+size > Usersize");
    }
    return Data.getRange(offset, size);
}

const BufferPtr Buffer::getRange(size_t offset, size_t size) const
{
    if (offset + size > Usersize)
    {
        throw InvalidParameterException("offset and size", "offset+size > Usersize");
    }
    return Data.getRange(offset, size);
}

SecureBuffer::~SecureBuffer()
{
    if (Data.get() != nullptr)
    {
        freeData();
    }
}

void SecureBuffer::erase()
{
#ifdef GST_NO_BURN_OPTIMISATION
    volatile size_t number_to_erase8 = Data.Size();
    volatile quint8* burnm8 = (volatile quint8*)(Data.Get());
    while (number_to_erase8--) { *burnm8++ = 0; }
#else
#ifndef GST_NO_COMPILER_INT64
    volatile size_t number_to_erase64 = Data.size() >> 3;
    volatile size_t number_to_erase8 = Data.size() % 8;
    volatile quint64* burnm64 = (volatile quint64*)(Data.get());
    volatile quint8* burnm8;
    while (number_to_erase64--) { *burnm64++ = 0; }
    burnm8 = (volatile quint8*) burnm64;
    while (number_to_erase8--) { *burnm8++ = 0; }
#else
    volatile size_t number_to_erase32 = Data.Size() >> 2;
    volatile size_t number_to_erase8 = Data.Size() % 4;
    volatile quint32* burnm32 = (volatile quint32*)(Data.Get());
    volatile quint8* burnm8;
    while (number_to_erase32--) { *burnm32++ = 0; }
    burnm8 = (volatile quint8*) burnm32;
    while (number_to_erase8--) { *burnm8++ = 0; }
#endif
#endif
}

void SecureBuffer::allocate(size_t size)
{
    Buffer::allocate(size);
    mlock(Data.get(), Data.size()); // locking mem to RAM
}

void SecureBuffer::freeData()
{
    if (Data.get() == nullptr)
    {
        throw BufferAlreadyFreedException();
    }

    erase();
    munlock(Data.get(), Data.size()); // do not forget to unlock memory
    Buffer::freeData();
}
}
