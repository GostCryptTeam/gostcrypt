/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

/**
*
*	\file MemoryStream.cpp
*	\brief
*	\version
*	\date
*
*/

#include "Exception.h"
#include "MemoryStream.h"

namespace GostCrypt
{
	MemoryStream::MemoryStream (const ConstBufferPtr &data) :
		ReadPosition (0)
	{
		Data = vector <byte> (data.Size());
		BufferPtr (&Data[0], Data.size()).CopyFrom (data);
	}

/**
 *
 *	\fn uint64 MemoryStream::Read (const BufferPtr &buffer)
 *	\brief
 *	\param[in] const BufferPtr
 *	\return
 *
 */
	uint64 MemoryStream::Read (const BufferPtr &buffer)
	{
		if (Data.size() == 0)
			throw ParameterIncorrect (SRC_POS);

		ConstBufferPtr streamBuf (*this);
		size_t len = buffer.Size();
		if (streamBuf.Size() - ReadPosition < len)
			len = streamBuf.Size() - ReadPosition;

		BufferPtr(buffer).CopyFrom (streamBuf.GetRange (ReadPosition, len));
		ReadPosition += len;
		return len;
	}

/**
 *
 *	\fn void MemoryStream::ReadCompleteBuffer (const BufferPtr &buffer)
 *	\brief
 *	\param[in] const BufferPtr
 *	\return
 *
 */
	void MemoryStream::ReadCompleteBuffer (const BufferPtr &buffer)
	{
		if (Read (buffer) != buffer.Size())
			throw InsufficientData (SRC_POS);
	}

/**
 *
 *	\fn void MemoryStream::Write (const ConstBufferPtr &data)
 *	\brief
 *	\param[in] const ConstBufferPtr
 *	\return
 *
 */
	void MemoryStream::Write (const ConstBufferPtr &data)
	{
		for (uint64 i = 0; i < data.Size(); i++)
			Data.push_back (data[i]);
	}
}
