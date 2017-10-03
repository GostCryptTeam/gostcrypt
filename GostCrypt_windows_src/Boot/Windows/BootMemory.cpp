/**
 *
 *	\file BootMemory.cpp
 *	\brief Manage BiosMemoryMapEntry object
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include "BootDefs.h"
#include "BootMemory.h"

static uint32 MemoryMapContValue;

/**
 *
 *	\fn static bool GetMemoryMapEntry (BiosMemoryMapEntry &entry)
 *	\brief Get the memory map entry
 *	\param[in] BiosMemoryMapEntry & entry
 *	\return static bool resultMagic == magic && resultSize == bufferSize
 *
 */
static bool GetMemoryMapEntry (BiosMemoryMapEntry &entry)
{
	static const uint32 function = 0x0000E820UL;
	static const uint32 magic = 0x534D4150UL;
	static const uint32 bufferSize = sizeof (BiosMemoryMapEntry);

	bool carry = false;
	uint32 resultMagic;
	uint32 resultSize;

	__asm
	{
		push es

		lea di, function
		GST_ASM_MOV_EAX_DI
		lea di, MemoryMapContValue
		GST_ASM_MOV_EBX_DI
		lea di, bufferSize
		GST_ASM_MOV_ECX_DI
		lea di, magic
		GST_ASM_MOV_EDX_DI
		lea di, MemoryMapContValue
		GST_ASM_MOV_DI_ECX

		// Use alternative segment to prevent memory corruption caused by buggy BIOSes
		push GST_BOOT_LOADER_ALT_SEGMENT
		pop es
		mov di, 0
		
		int 0x15
		jnc no_carry
		mov carry, true
	no_carry:

		lea di, resultMagic
		GST_ASM_MOV_DI_EAX
		lea di, MemoryMapContValue
		GST_ASM_MOV_DI_EBX
		lea di, resultSize
		GST_ASM_MOV_DI_ECX

		pop es
	}

	CopyMemory (GST_BOOT_LOADER_ALT_SEGMENT, 0, &entry, sizeof (entry));

	// BIOS may set CF at the end of the list
	if (carry)
		MemoryMapContValue = 0;

	return resultMagic == magic && resultSize == bufferSize;
}


/**
 *
 *	\fn bool GetFirstBiosMemoryMapEntry (BiosMemoryMapEntry &entry)
 *	\brief Get the first BiosMemoryMapEntry
 *	\param[in] BiosMemoryMapEntry & entry
 *	\return bool GetMemoryMapEntry (entry)
 *
 */
bool GetFirstBiosMemoryMapEntry (BiosMemoryMapEntry &entry)
{
	MemoryMapContValue = 0;
	return GetMemoryMapEntry (entry);
}


/**
 *
 *	\fn bool GetNextBiosMemoryMapEntry (BiosMemoryMapEntry &entry)
 *	\brief Get the next BiosMemoryMapEntry
 *	\param[in] BiosMemoryMapEntry & entry
 *	\return bool false if MemoryMapContValue == 0
 *			GetMemoryMapEntry (entry) else
 *
 */
bool GetNextBiosMemoryMapEntry (BiosMemoryMapEntry &entry)
{
	if (MemoryMapContValue == 0)
		return false;

	return GetMemoryMapEntry (entry);
}
