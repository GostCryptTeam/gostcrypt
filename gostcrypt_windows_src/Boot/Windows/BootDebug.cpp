/**
 *
 *	\file BootDebug.cpp
 *	\brief Manage debug functions
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

#include "Platform.h"
#include "Bios.h"
#include "BootConsoleIo.h"
#include "BootDefs.h"
#include "BootDiskIo.h"
#include "BootDebug.h"


#ifdef GST_BOOT_TRACING_ENABLED

/**
 *
 *	\fn void InitDebugPort ()
 *	\brief Initialize the debug port
 *	\return void
 *
 */
void InitDebugPort ()
{
	__asm
	{
		mov dx, GST_DEBUG_PORT
		mov ah, 1
		int 0x17
		mov dx, GST_DEBUG_PORT
		mov ah, 0xe2
		int 0x17
	}
}


/**
 *
 *	\fn void WriteDebugPort (byte dataByte)
 *	\brief Write on the debug port
 *	\param[in] byte dataByte
 *	\return void
 *
 */
void WriteDebugPort (byte dataByte)
{
	__asm
	{
		mov al, dataByte
		mov dx, GST_DEBUG_PORT
		mov ah, 0
		int 0x17
	}
}

#endif // GST_BOOT_TRACING_ENABLED


#ifdef GST_BOOT_DEBUG_ENABLED

/**
 *
 *	\fn extern "C" void PrintDebug (uint32 debugVal)
 *	\brief Print the debug output
 *	\param[in] uint32 debugVal
 *	\return void
 *
 */
extern "C" void PrintDebug (uint32 debugVal)
{
	Print (debugVal);
	PrintEndl();
}


/**
 *
 *	\fn void PrintVal (const char *message, const uint32 value, bool newLine, bool hex)
 *	\brief Print the message, the value (as hexa or not)and a new line if needed
 *	\param[in] bool hex
 *	\param[in] bool newLine
 *	\param[in] const uint32 value
 *	\param[in] const char * message
 *	\return void
 *
 */
void PrintVal (const char *message, const uint32 value, bool newLine, bool hex)
{
	Print (message);
	Print (": ");
	
	if (hex)
		PrintHex (value);
	else
		Print (value);
	
	if (newLine)
		PrintEndl();
}


/**
 *
 *	\fn void PrintVal (const char *message, const uint64 &value, bool newLine, bool hex)
 *	\brief Print the message, the value (as hexa) and a new line if needed
 *	\param[in] bool hex
 *	\param[in] bool newLine
 *	\param[in] const uint64 & value
 *	\param[in] const char * message
 *	\return void
 *
 */
void PrintVal (const char *message, const uint64 &value, bool newLine, bool hex)
{
	Print (message);
	Print (": ");
	PrintHex (value);
	if (newLine)
		PrintEndl();
}


/**
 *
 *	\fn void PrintHexDump (byte *mem, size_t size, uint16 *memSegment)
 *	\brief Print a dump as hexa
 *	\param[in] uint16 * memSegment
 *	\param[in] size_t size
 *	\param[in] byte * mem
 *	\return void
 *
 */
void PrintHexDump (byte *mem, size_t size, uint16 *memSegment)
{
	const size_t width = 16;
	for (size_t pos = 0; pos < size; )
	{
		for (int pass = 1; pass <= 2; ++pass)
		{
			size_t i;
			for (i = 0; i < width && pos < size; ++i)
			{
				byte dataByte;
				if (memSegment)
				{
					__asm
					{
						push es
						mov si, ss:memSegment
						mov es, ss:[si]
						mov si, ss:mem
						add si, pos
						mov al, es:[si]
						mov dataByte, al
						pop es
					}
					pos++;
				}
				else
					dataByte = mem[pos++];

				if (pass == 1)
				{
					PrintHex (dataByte);
					PrintChar (' ');
				}
				else
					PrintChar (IsPrintable (dataByte) ? dataByte : '.');
			}

			if (pass == 1)
			{
				pos -= i;
				PrintChar (' ');
			}
		}

		PrintEndl ();
	}
}


/**
 *
 *	\fn void PrintHexDump (uint16 memSegment, uint16 memOffset, size_t size)
 *	\brief Print dump as hexa
 *	\param[in] size_t size
 *	\param[in] uint16 memOffset
 *	\param[in] uint16 memSegment
 *	\return void
 *
 */
void PrintHexDump (uint16 memSegment, uint16 memOffset, size_t size)
{
	PrintHexDump ((byte *) memOffset, size, &memSegment);
}

#endif // GST_BOOT_DEBUG_ENABLED


#ifdef GST_BOOT_STACK_CHECKING_ENABLED

extern "C" char end[];

/**
 *
 *	\fn static void PrintStackInfo ()
 *	\brief Print the stack info
 *	\return static void
 *
 */
static void PrintStackInfo ()
{
	uint16 spReg;
	__asm mov spReg, sp

	Print ("Stack: "); Print (GST_BOOT_LOADER_STACK_TOP - spReg);
	Print ("/"); Print (GST_BOOT_LOADER_STACK_TOP - (uint16) end);
}


/**
 *
 *	\fn void CheckStack ()
 *	\brief Check the stack (in case of overflow)
 *	\return void
 *
 */
void CheckStack ()
{
	uint16 spReg;
	__asm mov spReg, sp

	if (*(uint32 *) end != 0x12345678UL || spReg < (uint16) end)
	{
		__asm cli
		__asm mov sp, GST_BOOT_LOADER_STACK_TOP

		PrintError ("Stack overflow");
		GST_THROW_FATAL_EXCEPTION;
	}
}


/**
 *
 *	\fn void InitStackChecker ()
 *	\brief Initialize the stack checker
 *	\return void
 *
 */
void InitStackChecker ()
{
	*(uint32 *) end = 0x12345678UL;

	PrintStackInfo();
	PrintEndl();
}

#endif // GST_BOOT_STACK_CHECKING_ENABLED