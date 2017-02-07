/**
 *
 *	\file Platform.cpp
 *	\brief Manage operators
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
#include "BootConsoleIo.h"


/**
 *
 *	\fn uint64 operator+ (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '+' between 2 uint64
 *	\param[in] const uint64 & a
 *	\param[in] const uint64 & b
 *	\return uint64 r
 *
 */
uint64 operator+ (const uint64 &a, const uint64 &b)
{
	int carry = 0;
	uint64 r;

	r.LowPart = a.LowPart + b.LowPart;
	__asm
	{
		jnc nocarry
		mov carry, 1
	nocarry:
	}

	r.HighPart = a.HighPart + b.HighPart + carry;

	return r;
}

/**
 *
 *	\fn uint64 operator+ (const uint64 &a, uint32 b)
 *	\brief Manage operator '+' with 1 uint64 with 1 uint32
 *	\param[in] uint32 b
 *	\param[in] const uint64 & a
 *	\return uint64 a + b64
 *
 */
uint64 operator+ (const uint64 &a, uint32 b)
{
	uint64 b64;
	b64.HighPart = 0;
	b64.LowPart = b;
	return a + b64;
}

/**
 *
 *	\fn uint64 &operator+= (uint64 &a, const uint64 &b)
 *	\brief Manage operator '+=' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] uint64 & a
 *	\return uint64 a = a + b
 *
 */
uint64 &operator+= (uint64 &a, const uint64 &b)
{
	return a = a + b;
}

/**
 *
 *	\fn uint64 operator- (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '-' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return uint64 r
 *
 */
uint64 operator- (const uint64 &a, const uint64 &b)
{
	int carry = 0;
	uint64 r;

	r.LowPart = a.LowPart - b.LowPart;
	__asm
	{
		jnc nocarry
		mov carry, 1
	nocarry:
	}

	r.HighPart = a.HighPart - b.HighPart - carry;

	return r;
}

/**
 *
 *	\fn uint64 operator- (const uint64 &a, uint32 b)
 *	\brief Manage operator '-' between 1 uint64 and 1 uint32
 *	\param[in] uint32 b
 *	\param[in] const uint64 & a
 *	\return uint64 a - b64
 *
 */
uint64 operator- (const uint64 &a, uint32 b)
{
	uint64 b64;
	b64.HighPart = 0;
	b64.LowPart = b;
	return a - b64;
}

/**
 *
 *	\fn uint64 &operator-= (uint64 &a, const uint64 &b)
 *	\brief Manage operator '-=' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] uint64 & a
 *	\return uint64 a = a - b
 *
 */
uint64 &operator-= (uint64 &a, const uint64 &b)
{
	return a = a - b;
}

/**
 *
 *	\fn uint64 operator>> (const uint64 &a, int shiftCount)
 *	\brief Manage operator '>>' between 1 uint64 and 1 int
 *	\param[in] int shiftCount
 *	\param[in] const uint64 & a
 *	\return uint64 r
 *
 */
uint64 operator>> (const uint64 &a, int shiftCount)
{
	uint64 r = a;

	while (shiftCount--)
	{
		r.LowPart >>= 1;
		
		if ((byte) r.HighPart & 1)
			r.LowPart |= 0x80000000UL;

		r.HighPart >>= 1;
	}

	return r;
}

/**
 *
 *	\fn uint64 operator<< (const uint64 &a, int shiftCount)
 *	\brief Manage operator '<<' between 1 uint64 and 1 int
 *	\param[in] int shiftCount
 *	\param[in] const uint64 & a
 *	\return uint64 r
 *
 */
uint64 operator<< (const uint64 &a, int shiftCount)
{
	uint64 r = a;
	
	while (shiftCount--)
		r += r;

	return r;
}

/**
 *
 *	\fn uint64 &operator++ (uint64 &a)
 *	\brief Manage operator '++' between uint64
 *	\param[in] uint64 & a
 *	\return uint64 a += b
 *
 */
uint64 &operator++ (uint64 &a)
{
	uint64 b;
	b.HighPart = 0;
	b.LowPart = 1;

	return a += b;
}

/**
 *
 *	\fn bool operator== (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '==' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return bool a.HighPart == b.HighPart && a.LowPart == b.LowPart
 *
 */
bool operator== (const uint64 &a, const uint64 &b)
{
	return a.HighPart == b.HighPart && a.LowPart == b.LowPart;
}

/**
 *
 *	\fn bool operator> (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '>' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return bool (a.HighPart > b.HighPart) || (a.HighPart == b.HighPart && a.LowPart > b.LowPart)
 *
 */
bool operator> (const uint64 &a, const uint64 &b)
{
	return (a.HighPart > b.HighPart) || (a.HighPart == b.HighPart && a.LowPart > b.LowPart);
}

/**
 *
 *	\fn bool operator< (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '<' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return bool (a.HighPart < b.HighPart) || (a.HighPart == b.HighPart && a.LowPart < b.LowPart)
 *
 */
bool operator< (const uint64 &a, const uint64 &b)
{
	return (a.HighPart < b.HighPart) || (a.HighPart == b.HighPart && a.LowPart < b.LowPart);
}

/**
 *
 *	\fn bool operator>= (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '>=' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return bool a > b || a == b
 *
 */
bool operator>= (const uint64 &a, const uint64 &b)
{
	return a > b || a == b;
}

/**
 *
 *	\fn bool operator<= (const uint64 &a, const uint64 &b)
 *	\brief Manage operator '<=' between 2 uint64
 *	\param[in] const uint64 & b
 *	\param[in] const uint64 & a
 *	\return bool a < b || a == b
 *
 */
bool operator<= (const uint64 &a, const uint64 &b)
{
	return a < b || a == b;
}

/**
 *
 *	\fn bool TestInt64 ()
 *	\brief Test an int64
 *	\return bool false if (c.HighPart != 0x112233UL || c.LowPart != 0xAABC0123UL)
 *			(c.HighPart == 0x22446755UL && c.LowPart == 0x78024600UL) else
 *
 */
bool TestInt64 ()
{
	uint64 a, b, c;
	a.HighPart = 0x00112233UL;
	a.LowPart = 0xabcd1234UL;

	b.HighPart = 0x00ffeeddUL;
	b.LowPart = 0xffffFFFFUL;

	a += b;
	a -= b;
	
	++a;
	
	b = b + (uint32) 1UL;

	c = (a - ((a + b) >> 32) - (uint32) 1UL);
	if (c.HighPart != 0x112233UL || c.LowPart != 0xAABC0123UL)
		return false;

	c = c << 9;
	return c.HighPart == 0x22446755UL && c.LowPart == 0x78024600UL;
}


/**
 *
 *	\fn void CopyMemory (void *source, uint16 destSegment, uint16 destOffset, uint16 blockSize)
 *	\brief Copy the memory
 *	\param[in] uint16 blockSize
 *	\param[in] uint16 destOffset
 *	\param[in] uint16 destSegment
 *	\param[in] void * source
 *	\return void
 *
 */
void CopyMemory (void *source, uint16 destSegment, uint16 destOffset, uint16 blockSize)
{
	__asm
	{
		push es
		mov si, ss:source
		mov es, ss:destSegment
		mov di, ss:destOffset
		mov cx, ss:blockSize
		cld
		rep movsb
		pop es
	}
}


/**
 *
 *	\fn void CopyMemory (uint16 sourceSegment, uint16 sourceOffset, void *destination, uint16 blockSize)
 *	\brief Copy the memory
 *	\param[in] uint16 blockSize
 *	\param[in] void * destination
 *	\param[in] uint16 sourceOffset
 *	\param[in] uint16 sourceSegment
 *	\return void
 *
 */
void CopyMemory (uint16 sourceSegment, uint16 sourceOffset, void *destination, uint16 blockSize)
{
	__asm
	{
		push ds
		push es
		mov ax, ds
		mov es, ax
		mov di, ss:destination
		mov si, ss:sourceOffset
		mov cx, ss:blockSize
		mov ds, ss:sourceSegment
		cld
		rep movsb
		pop es
		pop ds
	}
}


/**
 *
 *	\fn void EraseMemory (void *memory, int size)
 *	\brief Erase the memory
 *	\param[in] int size
 *	\param[in] void * memory
 *	\return void
 *
 */
void EraseMemory (void *memory, int size)
{
	memset (memory, 0, size);
}


/**
 *
 *	\fn uint32 GetLinearAddress (uint16 segment, uint16 offset)
 *	\brief Get a linear address
 *	\param[in] uint16 offset
 *	\param[in] uint16 segment
 *	\return uint32 (uint32 (segment) << 4) + offset
 *
 */
uint32 GetLinearAddress (uint16 segment, uint16 offset)
{
	return (uint32 (segment) << 4) + offset;
}


/**
 *
 *	\fn bool RegionsIntersect (const uint64 &start1, uint32 length1, const uint64 &start2, const uint64 &end2)
 *	\brief Check if there is an intersection
 *	\param[in] const uint64 & end2
 *	\param[in] const uint64 & start2
 *	\param[in] uint32 length1
 *	\param[in] const uint64 & start1
 *	\return bool false if (intersectStart > intersectEnd)
 *			((intersectEnd + 1UL - intersectStart).LowPart != 0) else 
 *
 */
bool RegionsIntersect (const uint64 &start1, uint32 length1, const uint64 &start2, const uint64 &end2)
{
	uint64 end1 = start1 + length1 - 1UL;
	uint64 intersectEnd = (end1 <= end2) ? end1 : end2;
	
	uint64 intersectStart = (start1 >= start2) ? start1 : start2;
	if (intersectStart > intersectEnd)
		return false;
		
	return (intersectEnd + 1UL - intersectStart).LowPart != 0;
}


/**
 *
 *	\fn void ThrowFatalException (int line)
 *	\brief Throw a fatal Exception
 *	\param[in] int line
 *	\return void
 *
 */
void ThrowFatalException (int line)
{
	PrintChar ('#'); Print (line);
	while (1);
}
