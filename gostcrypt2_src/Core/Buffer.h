#ifndef GST_HEADER_Platform_Buffer
#define GST_HEADER_Platform_Buffer

#include <QtGlobal>
#include "Volume/VolumeException.h"

namespace GostCrypt {

class BufferPtr
{
 public:
    BufferPtr()
        : DataPtrMutable(nullptr), DataPtrReadonly(nullptr), DataSize(0) { }
    BufferPtr(quint8* data, size_t size)
        : DataPtrMutable(data), DataPtrReadonly(data), DataSize(size) { }
    BufferPtr(const quint8* data, size_t
              size)  // const version. should not be used with a non-const object
        : DataPtrMutable(nullptr), DataPtrReadonly(data), DataSize(size) { }
    virtual ~BufferPtr() { }

    operator const quint8* () const { return DataPtrReadonly; }  // const version does not allow any changes
    operator quint8* () { if (DataPtrReadonly != DataPtrMutable) { throw DataNotMutableException(); } return DataPtrMutable; }
    virtual bool IsDataEqual(const BufferPtr& otherbufferptr) const;

    void CopyFrom(const BufferPtr& bufferPtr);
    void Erase() { if (!DataPtrMutable) { throw DataNotMutableException(); } memset(DataPtrMutable, 0, DataSize); }

    quint8* Get() { if (DataPtrReadonly != DataPtrMutable) { throw DataNotMutableException(); } return DataPtrMutable; }
    const quint8* Get() const { return DataPtrReadonly; }  // const version

    BufferPtr GetRange(size_t offset, size_t size);
    const BufferPtr GetRange(size_t offset, size_t size) const;  // const version

    void Set(quint8* data, size_t size) { DataPtrMutable = data; DataPtrReadonly = data; DataSize = size; }
    size_t Size() const { return DataSize; }

 protected:
    quint8* DataPtrMutable;
    const quint8* DataPtrReadonly;
    size_t DataSize;
};

class SecureBuffer; // used to disallow cast from SecureBuffer to Buffer
class Buffer
{
 public:
    Buffer() : Data(), Usersize(0) {}
    Buffer(size_t size);
    Buffer(const BufferPtr& bufferPtr) { CopyFrom(bufferPtr); }
    virtual ~Buffer();

    virtual void Allocate(size_t size);
    virtual void CopyFrom(const BufferPtr& bufferPtr);
    virtual void Erase();
    virtual void Free();

    virtual size_t Size() const { return Usersize; }
    virtual bool IsAllocated() const { return Data.Size() != 0; }

    virtual quint8* Get() { return Data.Get(); }
    virtual const quint8* Get() const { return Data.Get(); }  // const version

    virtual BufferPtr GetRange(size_t offset, size_t size);
    virtual const BufferPtr GetRange(size_t offset, size_t size) const;  // const version

    virtual operator quint8* () { return Data.Get(); }
    virtual operator const quint8* () const { return Data.Get(); }

    virtual operator BufferPtr& () { return Data; }
    virtual operator const BufferPtr& () const { return Data; }

 protected:
    BufferPtr Data;
    size_t Usersize; // actual used size. (optimisation)
 private:
    Buffer(const Buffer&);
    Buffer(const SecureBuffer&);   // used to disallow cast from SecureBuffer to Buffer
    Buffer& operator= (const Buffer&);
};

class SecureBuffer : public Buffer
{
 public:
    SecureBuffer() { }
    SecureBuffer(size_t size) { Allocate(size); }
    SecureBuffer(const BufferPtr& bufferPtr) { CopyFrom(bufferPtr); }
    virtual ~SecureBuffer();

    virtual void Erase();
    virtual void Allocate(size_t size);
    virtual void Free();

 private:
    SecureBuffer(const SecureBuffer&);
    SecureBuffer& operator= (const SecureBuffer&);
};

}

#endif // GST_HEADER_Platform_Buffer
