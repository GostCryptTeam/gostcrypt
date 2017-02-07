/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

/**
*
*	\file Memory.cpp
*	\brief
*	\version
*	\date
*
*/

#include "Common/Gstdefs.h"
#include "Memory.h"
#include "Exception.h"

namespace GostCrypt
{
/**
 *
 *	\fn void *Memory::Allocate (std::size_t size)
 *	\brief
 *	\param[in] std::size_t size)
 *	\return
 *
 */
	void *Memory::Allocate (std::size_t size)
	{
		if (size < 1)
			throw ParameterIncorrect (SRC_POS);

		void *bufPtr = malloc (size);
		if (!bufPtr)
			throw bad_alloc();

		return bufPtr;
	}

/**
 *
 *	\fn int Memory::Compare (const void *memory1, size_t size1, const void *memory2, size_t size2)
 *	\brief
 *	\param[in] size_t size2)
 *	\param[in] const void
 *	\param[in] size_t size1
 *	\param[in] const void
 *	\return
 *
 */
	int Memory::Compare (const void *memory1, size_t size1, const void *memory2, size_t size2)
	{
		if (size1 > size2)
			return 1;
		else if (size1 < size2)
			return -1;
		
		return memcmp (memory1, memory2, size1);
	}

/**
 *
 *	\fn void Memory::Copy (void *memoryDestination, const void *memorySource, size_t size)
 *	\brief
 *	\param[in] size_t size)
 *	\param[in] const void
 *	\param[in] void* memoryDestination
 *	\return
 *
 */
	void Memory::Copy (void *memoryDestination, const void *memorySource, size_t size)
	{
		assert (memoryDestination != nullptr && memorySource != nullptr);
		memcpy (memoryDestination, memorySource, size);
	}

/**
 *
 *	\fn void Memory::Erase (void *memory, size_t size)
 *	\brief
 *	\param[in] size_t size)
 *	\param[in] void* memory
 *	\return
 *
 */
	void Memory::Erase (void *memory, size_t size)
	{
		burn (memory, size);
	}

/**
 *
 *	\fn void Memory::Zero (void *memory, size_t size)
 *	\brief
 *	\param[in] size_t size)
 *	\param[in] void* memory
 *	\return
 *
 */
	void Memory::Zero (void *memory, size_t size)
	{
		memset (memory, 0, size);
	}

/**
 *
 *	\fn void Memory::Free (void *memory)
 *	\brief
 *	\param[in] void* memory)
 *	\return
 *
 */
	void Memory::Free (void *memory)
	{
		assert (memory != nullptr);
		free (memory);
	}
}
