/**
 *
 *	\file BootConsoleIo.cpp
 *	\brief Manage the output
 *	\version 1.3.1
 *	\date 13/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include "Platform.h"
#include "Bios.h"
#include "BootConsoleIo.h"
#include "BootDebug.h"
#include "BootStrings.h"


static int ScreenOutputDisabled = 0;

/**
 *
 *	\fn void DisableScreenOutput ()
 *	\brief Disable the screen output (ScreenOutputDisabled = 1)
 *	\return void
 *
 */
void DisableScreenOutput ()
{
	++ScreenOutputDisabled;
}


/**
 *
 *	\fn void EnableScreenOutput ()
 *	\brief Enable the screen output (ScreenOutputDisabled = 0)
 *	\return void
 *
 */
void EnableScreenOutput ()
{
	--ScreenOutputDisabled;
}


/**
 *
 *	\fn void PrintChar (char c)
 *	\brief Print char c if screen output is enabled
 *	\param[in] char c
 *	\return void
 *
 */
void PrintChar (char c)
{
#ifdef GST_BOOT_TRACING_ENABLED
	WriteDebugPort (c);
#endif

	if (ScreenOutputDisabled)
		return;

	__asm
	{
		mov bx, 7
		mov al, c
		mov ah, 0xe
		int 0x10
	}
}


/**
 *
 *	\fn void PrintCharAtCursor (char c)
 *	\brief Print char at cursor c if screen output is enabled
 *	\param[in] char c
 *	\return void
 *
 */
void PrintCharAtCursor (char c)
{
	if (ScreenOutputDisabled)
		return;

	__asm
	{
		mov bx, 7
		mov al, c
		mov cx, 1
		mov ah, 0xa
		int 0x10
	}
}


/**
 *
 *	\fn void Print (const char *str)
 *	\brief Call Printchar to print the string char by char
 *	\param[in] const char * str
 *	\return void
 *
 */
void Print (const char *str)
{
	char c;
	while (c = *str++)
		PrintChar (c);
}


/**
 *
 *	\fn void Print (uint32 number)
 *	\brief Convert number to string and call Printchar to print the string char by char
 *	\param[in] uint32 number
 *	\return void
 *
 */
void Print (uint32 number)
{
	char str[12];
	int pos = 0;
	while (number >= 10)
	{
		str[pos++] = (char) (number % 10) + '0';
		number /= 10;
	}
	str[pos] = (char) (number % 10) + '0';
	
	while (pos >= 0)
		PrintChar (str[pos--]);
}


/**
 *
 *	\fn void Print (const uint64 &number)
 *	\brief Print a uint64
 *	\param[in] const uint64 & number
 *	\return void
 *
 */
void Print (const uint64 &number)
{
	if (number.HighPart == 0)
		Print (number.LowPart);
	else
		PrintHex (number);
}


/**
 *
 *	\fn void PrintHex (byte b)
 *	\brief Call PrintChar with hexa values
 *	\param[in] byte b
 *	\return void
 *
 */
void PrintHex (byte b)
{
	PrintChar (((b >> 4) >= 0xA ? 'A' - 0xA : '0') + (b >> 4));
	PrintChar (((b & 0xF) >= 0xA ? 'A' - 0xA : '0') + (b & 0xF));
}


/**
 *
 *	\fn void PrintHex (uint16 data)
 *	\brief Print a uint16 as 2 uint8
 *	\param[in] uint16 data
 *	\return void
 *
 */
void PrintHex (uint16 data)
{
	PrintHex (byte (data >> 8));
	PrintHex (byte (data));
}


/**
 *
 *	\fn void PrintHex (uint32 data)
 *	\brief Print uint32 as 2 uint16
 *	\param[in] uint32 data
 *	\return void
 *
 */
void PrintHex (uint32 data)
{
	PrintHex (uint16 (data >> 16));
	PrintHex (uint16 (data));
}


/**
 *
 *	\fn void PrintHex (const uint64 &data)
 *	\brief Print a uint63 as 2 uint32
 *	\param[in] const uint64 & data
 *	\return void
 *
 */
void PrintHex (const uint64 &data)
{
	PrintHex (data.HighPart);
	PrintHex (data.LowPart);
}

/**
 *
 *	\fn void PrintRepeatedChar (char c, int n)
 *	\brief Print char c, n times
 *	\param[in] int n
 *	\param[in] char c
 *	\return void
 *
 */
void PrintRepeatedChar (char c, int n)
{
	while (n-- > 0)
		PrintChar (c);
}


/**
 *
 *	\fn void PrintEndl ()
 *	\brief Print the end of line
 *	\return void
 *
 */
void PrintEndl ()
{
	Print ("\r\n");
}


/**
 *
 *	\fn void PrintEndl (int cnt)
 *	\brief Print the end of line cnt times
 *	\param[in] int cnt
 *	\return void
 *
 */
void PrintEndl (int cnt)
{
	while (cnt-- > 0)
		PrintEndl ();
}


/**
 *
 *	\fn void Beep ()
 *	\brief Make a beep
 *	\return void
 *
 */
void Beep ()
{
	PrintChar (7);
}


/**
 *
 *	\fn void InitVideoMode ()
 *	\brief Initialize the video mode
 *	\return void
 *
 */
void InitVideoMode ()
{
	if (ScreenOutputDisabled)
		return;

	__asm
	{
		// Text mode 80x25
		mov ax, 3
		int 0x10

		// Page 0
		mov ax, 0x500
		int 0x10
	}
}


/**
 *
 *	\fn void ClearScreen ()
 *	\brief Clear the screen if screen output is enabled
 *	\return void
 *
 */
void ClearScreen ()
{
	if (ScreenOutputDisabled)
		return;

	__asm
	{
		// White text on black
		mov bh, 7
		xor cx, cx
		mov dx, 0x184f
		mov ax, 0x600
		int 0x10

		// Cursor at 0,0
		xor bh, bh
		xor dx, dx
		mov ah, 2
		int 0x10
	}
}


/**
 *
 *	\fn void PrintBackspace ()
 *	\brief Print a backspaace
 *	\return void
 *
 */
void PrintBackspace ()
{
	PrintChar (GST_BIOS_CHAR_BACKSPACE);
	PrintCharAtCursor (' ');
}


/**
 *
 *	\fn void PrintError (const char *message)
 *	\brief Print error with his message
 *	\param[in] const char * message
 *	\return void
 *
 */
void PrintError (const char *message)
{
	Print (GST_BOOT_STR_ERROR);
	Print (message);
	PrintEndl();
	Beep();
}


/**
 *
 *	\fn void PrintErrorNoEndl (const char *message)
 *	\brief Print error with his message without the end of line
 *	\param[in] const char * message
 *	\return void
 *
 */
void PrintErrorNoEndl (const char *message)
{
	Print (GST_BOOT_STR_ERROR);
	Print (message);
	Beep();
}


/**
 *
 *	\fn byte GetShiftFlags ()
 *	\brief Get shift flags
 *	\return byte flags
 *
 */
byte GetShiftFlags ()
{
	byte flags;
	__asm
	{
		mov ah, 2
		int 0x16
		mov flags, al
	}

	return flags;
}


/**
 *
 *	\fn byte GetKeyboardChar ()
 *	\brief Get keyboard char in ascii
 *	\return byte GetKeyboardChar(nullptr)
 *
 */
byte GetKeyboardChar ()
{
	return GetKeyboardChar (nullptr);
}


/**
 *
 *	\fn byte GetKeyboardChar (byte *scanCode)
 *	\brief Get ascii code of keyboard char
 *	\param[in] byte * scanCode
 *	\return byte asciiCode
 *
 */
byte GetKeyboardChar (byte *scanCode)
{
	// Work around potential BIOS bugs (Windows boot manager polls the keystroke buffer)
	while (!IsKeyboardCharAvailable());

	byte asciiCode;
	byte scan;
	__asm
	{
		mov ah, 0
		int 0x16
		mov asciiCode, al
		mov scan, ah
	}
	
	if (scanCode)
		*scanCode = scan;

	return asciiCode;
}


/**
 *
 *	\fn bool IsKeyboardCharAvailable ()
 *	\brief Check if keyboard char is available
 *	\return bool true if the keyboard char is available, false else
 *
 */
bool IsKeyboardCharAvailable ()
{
	bool available = false;
	__asm
	{
		mov ah, 1
		int 0x16
		jz not_avail
		mov available, true
	not_avail:
	}

	return available;
}


/**
 *
 *	\fn bool EscKeyPressed ()
 *	\brief Check if escape key is pressed
 *	\return bool true if escape is pressed
 *			false else
 *
 */
bool EscKeyPressed ()
{
	if (IsKeyboardCharAvailable ())
	{
		byte keyScanCode;
		GetKeyboardChar (&keyScanCode);
		return keyScanCode == GST_BIOS_KEY_ESC;
	}

	return false;
}


/**
 *
 *	\fn void ClearBiosKeystrokeBuffer ()
 *	\brief Clear the bios key stroke buffer
 *	\return void
 *
 */
void ClearBiosKeystrokeBuffer ()
{
	__asm
	{
		push es
		xor ax, ax
		mov es, ax
		mov di, 0x41e
		mov cx, 32
		cld
		rep stosb
		pop es
	}
}


/**
 *
 *	\fn bool IsPrintable (char c)
 *	\brief Is the char c printable ?
 *	\param[in] char c
 *	\return bool c >= ' ' && c <= '~'
 *
 */
bool IsPrintable (char c)
{
	return c >= ' ' && c <= '~';
}


/**
 *
 *	\fn int GetString (char *buffer, size_t bufferSize)
 *	\brief Print the string buffer and return the position when escape key is pressed or at the end of the buffer
 *	\param[in] size_t bufferSize
 *	\param[in] char * buffer
 *	\return int 0 if escape is pressed
 *			pos else
 *
 */
int GetString (char *buffer, size_t bufferSize)
{
	byte c;
	byte scanCode;
	size_t pos = 0;

	while (pos < bufferSize)
	{
		c = GetKeyboardChar (&scanCode);

		if (scanCode == GST_BIOS_KEY_ENTER)
			break;
		
		if (scanCode == GST_BIOS_KEY_ESC)
			return 0;

		buffer[pos++] = c;
		PrintChar (IsPrintable (c) ? c : ' ');
	}

	return pos;
}
