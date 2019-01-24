#ifndef GST_HEADER_Platform_Buffer
#define GST_HEADER_Platform_Buffer

#include <QtGlobal>
#include "Volume/VolumeException.h"

namespace GostCrypt
{

class SecureBuffer;

/**
 * @brief Class handling pointer to Buffer data
 *
 */
class BufferPtr
{
 public:
    /**
     * @brief Default constructor creating a null pointer
     *
     */
    BufferPtr() : dataPtrMutable(NULL), dataPtrReadonly(NULL), dataSize(0) {}
    /**
     * @brief Constructor initializing the BufferPtr with a raw data pointer and a data size.
     *
     * @param data Pointer to the buffer data
     * @param size Size of the pointed buffer
     * @warning You will have to free the memory yourself
     */
    BufferPtr(quint8* data, size_t size)
        : dataPtrMutable(data), dataPtrReadonly(data), dataSize(size) { }
    /**
     * @brief Constructor initializing the BufferPtr with a const raw data pointer and a data size. Used to make sure the data pointed cannot be modified.
     *
     * @param data Pointer to the const buffer data
     * @param size Size of the pointed buffer
     * @warning You will have to free the memory yourself
     * @warning Well initializing with const data the BufferPtr should be const
     */
    BufferPtr(const quint8* data, size_t
              size)  // const version. should not be used with a non-const object
        : dataPtrMutable(nullptr), dataPtrReadonly(data), dataSize(size) { }
    /**
     * @brief Default destructor. Doing nothing since the BufferPtr point to a Buffer object in charge of freeing its memory
     *
     */
    virtual ~BufferPtr() { }

    /**
     * @brief Conversion operator to get the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    operator const quint8* () const { return dataPtrReadonly; }
    /**
     * @brief Conversion operator to get the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     * @warning Will throw a DataNotMutableException if the BufferPtr points to const data
     */
    operator quint8* () { if (dataPtrReadonly != dataPtrMutable) { throw DataNotMutableException(); } return dataPtrMutable; }
    /**
     * @brief Compare data stored in the BufferPtr with the data of another BufferPtr
     *
     * @param otherbufferptr Other BufferPtr to compare data with
     * @return bool Return True is the data of BufferPtrs are equals
     */
    virtual bool isDataEqual(const BufferPtr& otherbufferptr) const;

    /**
     * @brief Copy Data from another BufferPtr to this BufferPtr
     * @warning Will throw a IncorrectParameterException if this BufferPtr does have enough space to store the data of the other BufferPtr
     * @warning Will throw a DataNotMutableException if this BufferPtr point to const data
     * @param bufferPtr Other BufferPtr to copy data from
     */
    void copyFrom(const BufferPtr& bufferPtr);
    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     * @warning Will throw a DataNotMutableException if this BufferPtr point to const data
     *
     */
    void erase() { if (!dataPtrMutable) { throw DataNotMutableException(); } memset(dataPtrMutable, 0, dataSize); }

    /**
     * @brief Return the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the const version of this method will be called when using a const BufferPtr
     */
    quint8* get() { if (dataPtrReadonly != dataPtrMutable) { throw DataNotMutableException(); } return dataPtrMutable; }
    /**
     * @brief Return the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    const quint8* get() const { return dataPtrReadonly; }  // const version

    /**
     * @brief Return new BufferPtr corresponding to a subset of the data of this BufferPtr
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return New BufferPtr corresponding to a subset of the data of this BufferPtr
     * @warning Will throw a IncorrectParameterException if requestData is out of the data of this BufferPtr
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the const version of this method will be called when using a const BufferPtr
     */
    BufferPtr getRange(size_t offset, size_t size);
    /**
     * @brief Return new const BufferPtr corresponding to a subset of the const data of this BufferPtr
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return New const BufferPtr corresponding to a subset of the const data of this BufferPtr
     */
    const BufferPtr getRange(size_t offset, size_t size) const;  // const version

    /**
     * @brief Make this BufferPtr points to other data
     *
     * @param data Pointer to the buffer new data
     * @param size Size of the pointed buffer
     */
    void set(quint8* data, size_t size) { dataPtrMutable = data; dataPtrReadonly = data; dataSize = size; }
    /**
     * @brief Return the size of this buffer in bytes
     *
     * @return size_t Size of this buffer in bytes
     */
    size_t size() const { return dataSize; }

 protected:
    quint8* dataPtrMutable; /**< Raw pointer to the data of this BufferPtr used when the data is not const */
    const quint8* dataPtrReadonly; /**< Raw pointer to the data of this BufferPtr */
    size_t dataSize; /**< Size of this buffer in bytes */
};

/**
 * @brief Class handling buffer data
 *
 */
class Buffer
{
 public:
    /**
     * @brief Default constructor initializing an empty buffer
     *
     */
    Buffer() : Data(), Usersize(0) {}
    /**
     * @brief Constructor initializing a buffer with given size
     *
     * @param size Size of the new Buffer in bytes
     */
    Buffer(size_t size);
    /**
     * @brief Deep copy constructor initializing a new Buffer with the same data as the given BufferPtr
     *
     * @param bufferPtr BufferPtr to copy the data from
     */
    Buffer(const BufferPtr& bufferPtr) { copyFrom(bufferPtr); }
    /**
     * @brief Default Destructor freeing the buffer data
     *
     */
    virtual ~Buffer();

    /**
     * @brief Allocate new memory for this Buffer
     * @warning The previsous data stored in the Buffer will not be kept
     * @param size New size of the buffer in bytes
     */
    virtual void allocate(size_t size);
    /**
     * @brief Copy Data from a BufferPtr to this Buffer
     * @warning Will throw a IncorrectParameterException if this BufferPtr does have enough space to store the data of the other BufferPtr
     * @param bufferPtr BufferPtr to copy data from
     */
    virtual void copyFrom(const BufferPtr& bufferPtr);
    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     *
     */
    virtual void erase();
    /**
     * @brief Free the data of this Buffer and set its size to 0
     *
     */
    virtual void freeData();

    /**
     * @brief Return the size of this buffer in bytes
     *
     * @return size_t Size of this buffer in bytes
     */
    virtual size_t size() const { return Usersize; }
    /**
     * @brief Return true if this Buffer size is not zero
     *
     * @return True if this Buffer size is not zero
     */
    virtual bool isAllocated() const { return Data.size() != 0; }

    /**
     * @brief Return the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     */
    virtual quint8* get() { return Data.get(); }
    /**
     * @brief Return the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    virtual const quint8* get() const { return Data.get(); }  // const version

    /**
     * @brief Return BufferPtr corresponding to a subset of the data of this Buffer
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return BufferPtr corresponding to a subset of the data of this Buffer
     * @warning Will throw a IncorrectParameterException if requestData is out of the data of this BufferPtr
     * @warning Will throw DataNotMutableException if this BufferPtr point to const data. Should not happened since the const version of this method will be called when using a const BufferPtr
     */
    virtual BufferPtr getRange(size_t offset, size_t size);

    /**
     * @brief Return const BufferPtr corresponding to a subset of the const data of this Buffer
     *
     * @param offset Offset in bytes before the subset of data requested
     * @param size Size of the subset of data requested
     * @return const BufferPtr corresponding to a subset of the const data of this Buffer
     */
    const BufferPtr getRange(size_t offset, size_t size) const;


    /**
     * @brief Conversion operator to get the raw pointer to the data of the Buffer
     *
     * @return Raw pointer to the data of the Buffer
     */
    virtual operator quint8* () { return Data.get(); }
    /**
     * @brief Conversion operator to get the raw pointer to the const data of the Buffer
     *
     * @return Raw pointer to the const data of the Buffer
     */
    virtual operator const quint8* () const { return Data.get(); }

    /**
     * @brief Conversion operator to get the corresponding BufferPtr
     *
     * @return corresponding BufferPtr
     */
    virtual operator BufferPtr& () { return Data; }
    /**
     * @brief Conversion operator to get the corresponding const BufferPtr
     *
     * @return corresponding const BufferPtr
     */
    virtual operator const BufferPtr& () const { return Data; }

 protected:
    BufferPtr Data; /**< Pointer to the buffer data handling allocated memory for the buffer */
    size_t Usersize; /**< Actual used size. Allow not to realloc when reducing the size of the Buffer*/
 private:
    /**
     * @brief The copy constructor is disabled to prevent heavy deep copy
     */
    Buffer(const Buffer&);
    /**
     * @brief The cast from SecureBuffer is obviously disallowed
     */
    Buffer(const SecureBuffer&);
    /**
     * @brief The equal operator is disabled to prevent heavy deep copy
     */
    Buffer& operator= (const Buffer&);
};

/**
 * @brief Class handling buffer data when data is sensitive. The data will be locked into RAM using mlock and erased before beeing freed
 *
 */
class SecureBuffer : public Buffer
{
 public:
    /**
     * @brief Default constructor initializing an empty buffer
     *
     */
    SecureBuffer() { }
    /**
     * @brief Constructor initializing a buffer with given size
     *
     * @param size Size of the new Buffer in bytes
     */
    SecureBuffer(size_t size) { allocate(size); }
    /**
     * @brief Deep copy constructor initializing a new Buffer with the same data as the given BufferPtr
     *
     * @param bufferPtr BufferPtr to copy the data from
     */
    SecureBuffer(const BufferPtr& bufferPtr) { copyFrom(bufferPtr); }
    /**
     * @brief Default Destructor erasing and freeing the buffer data
     *
     */
    virtual ~SecureBuffer();

    /**
     * @brief Will set the all data bytes of this BufferPtr to zero.
     *
     */
    virtual void erase();
    /**
     * @brief Allocate new memory for this Buffer and lock it in RAM using mlock
     * @warning The previsous data stored in the Buffer will not be kept
     * @param size New size of the buffer in bytes
     */
    virtual void allocate(size_t size);
    /**
     * @brief Free the data of this Buffer after having erased it and set its size to 0
     *
     */
    virtual void freeData();

 private:
    /**
     * @brief The copy constructor is disabled to prevent heavy deep copy
     */
    SecureBuffer(const SecureBuffer&);
    /**
     * @brief The equal operator is disabled to prevent heavy deep copy
     */
    SecureBuffer& operator= (const SecureBuffer&);
};

}

#endif // GST_HEADER_Platform_Buffer
