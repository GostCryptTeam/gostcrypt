/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_Buffer
#define GST_HEADER_Platform_Buffer

#include <QtGlobal>
#include "Memory.h"

namespace GostCrypt
{
	class ConstBufferPtr
	{
	public:
		ConstBufferPtr ()
			: DataPtr (nullptr), DataSize (0) { }
        ConstBufferPtr (const quint8 *data, size_t size)
			: DataPtr (data), DataSize (size) { }
		virtual ~ConstBufferPtr () { }

        operator const quint8 * () const { return DataPtr; }

		bool IsDataEqual (const ConstBufferPtr &other) const { return Memory::Compare (DataPtr, DataSize, other.DataPtr, other.DataSize) == 0; }
        const quint8 *Get () const { return DataPtr; }
		ConstBufferPtr GetRange (size_t offset, size_t size) const;
        void Set (const quint8 *data, size_t size) { DataPtr = data; DataSize = size; }
		size_t Size () const { return DataSize; }

	protected:
        const quint8 *DataPtr;
		size_t DataSize;
	};


	class BufferPtr
	{
	public:
		BufferPtr ()
			: DataPtr (nullptr), DataSize (0) { }
        BufferPtr (quint8 *data, size_t size)
			: DataPtr (data), DataSize (size) { }
		virtual ~BufferPtr () { }

        operator quint8 * () const { return DataPtr; }
		void CopyFrom (const ConstBufferPtr &bufferPtr) const;
		void Erase () const { Zero(); }
        quint8 *Get () const { return DataPtr; }
		BufferPtr GetRange (size_t offset, size_t size) const;
        void Set (quint8 *data, size_t size) { DataPtr = data; DataSize = size; }
		size_t Size () const { return DataSize; }
		void Zero () const { Memory::Zero (DataPtr, DataSize); }

		operator ConstBufferPtr () const { return ConstBufferPtr (DataPtr, DataSize); }

	protected:
        quint8 *DataPtr;
		size_t DataSize;
	};

	class Buffer
	{
	public:
		Buffer ();
		Buffer (size_t size);
		Buffer (const ConstBufferPtr &bufferPtr) { CopyFrom (bufferPtr); }
		virtual ~Buffer ();

		virtual void Allocate (size_t size);
		virtual void CopyFrom (const ConstBufferPtr &bufferPtr);
        virtual quint8 *Ptr () const { return DataPtr; }
		virtual void Erase ();
		virtual void Free ();
		virtual BufferPtr GetRange (size_t offset, size_t size) const;
		virtual size_t Size () const { return DataSize; }
		virtual bool IsAllocated () const { return DataSize != 0; }
		virtual void Zero ();

        virtual operator quint8 * () const { return DataPtr; }
		virtual operator BufferPtr () const { return BufferPtr (DataPtr, DataSize); }
		virtual operator ConstBufferPtr () const { return ConstBufferPtr (DataPtr, DataSize); }

	protected:
        quint8 *DataPtr;
		size_t DataSize;

	private:
		Buffer (const Buffer &);
		Buffer &operator= (const Buffer &);
	};

	class SecureBuffer : public Buffer
	{
	public:
		SecureBuffer () { }
		SecureBuffer (size_t size);
		SecureBuffer (const ConstBufferPtr &bufferPtr) { CopyFrom (bufferPtr); }
		virtual ~SecureBuffer ();

		virtual void Allocate (size_t size);
		virtual void Free ();

	private:
		SecureBuffer (const SecureBuffer &);
		SecureBuffer &operator= (const SecureBuffer &);
	};

}

#endif // GST_HEADER_Platform_Buffer
