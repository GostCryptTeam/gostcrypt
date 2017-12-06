/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_Memory
#define GST_HEADER_Platform_Memory

#include <new>
#include <memory.h>
#include <QtGlobal>

#ifndef BYTE_ORDER

#	ifdef GST_MACOSX
#		include <machine/endian.h>
#	elif defined (GST_BSD)
#		include <sys/endian.h>
#	elif defined (GST_SOLARIS)
#		include <sys/types.h>
#		define LITTLE_ENDIAN 1234
#		define BIG_ENDIAN 4321
#		ifdef _BIG_ENDIAN
#			define BYTE_ORDER BIG_ENDIAN
#		else
#			define BYTE_ORDER LITTLE_ENDIAN
#		endif
#	else
#		include <endian.h>
#	endif

#	ifndef BYTE_ORDER
#		ifndef __BYTE_ORDER
#			error Byte ordering cannot be determined (BYTE_ORDER undefined).
#		endif

#		define BYTE_ORDER __BYTE_ORDER
#	endif

#	ifndef LITTLE_ENDIAN
#		define LITTLE_ENDIAN __LITTLE_ENDIAN
#	endif

#	ifndef BIG_ENDIAN
#		define BIG_ENDIAN __BIG_ENDIAN
#	endif

#endif // !BYTE_ORDER

#if BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN
#	error Unsupported byte ordering detected.
#endif

namespace GostCrypt
{
	class Memory
	{
	public:
		static void *Allocate (size_t size);
		static int Compare (const void *memory1, size_t size1, const void *memory2, size_t size2);
		static void Copy (void *memoryDestination, const void *memorySource, size_t size);
		static void Erase (void *memory, size_t size);
		static void Free (void *memory);
		static void Zero (void *memory, size_t size);
	};
}

#endif // GST_HEADER_Platform_Memory
