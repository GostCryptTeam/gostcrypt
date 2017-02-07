/*
 Copyright (c) 2008-2009 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Boot_Platform
#define GST_HEADER_Boot_Platform

#pragma warning (disable: 4018 4102 4704 4769)

#include "Gstdefs.h"
#include <memory.h>

typedef char bool;
#define false 0
#define true 1

#define nullptr 0
#define NULL 0

typedef UINT64_STRUCT uint64;

#define array_capacity(arr) (sizeof (arr) / sizeof ((arr)[0]))

#define GST_TO_STRING2(n) #n
#define GST_TO_STRING(n) GST_TO_STRING2(n)


#define GST_X86_CARRY_FLAG 0x1

#define GST_ASM_EMIT(A,B) __asm _emit 0x##A __asm _emit 0x##B
#define GST_ASM_EMIT3(A,B,C) __asm _emit 0x##A __asm _emit 0x##B __asm _emit 0x##C
#define GST_ASM_EMIT4(A,B,C,D) __asm _emit 0x##A __asm _emit 0x##B __asm _emit 0x##C __asm _emit 0x##D 

#define GST_ASM_MOV_EAX_DI GST_ASM_EMIT3 (66, 8B, 05)
#define GST_ASM_MOV_EBX_DI GST_ASM_EMIT3 (66, 8B, 1D)
#define GST_ASM_MOV_ECX_DI GST_ASM_EMIT3 (66, 8B, 0D)
#define GST_ASM_MOV_EDX_DI GST_ASM_EMIT3 (66, 8B, 15)

#define GST_ASM_MOV_DI_EAX GST_ASM_EMIT3 (66, 89, 05)
#define GST_ASM_MOV_DI_EBX GST_ASM_EMIT3 (66, 89, 1D)
#define GST_ASM_MOV_DI_ECX GST_ASM_EMIT3 (66, 89, 0D)
#define GST_ASM_MOV_DI_EDX GST_ASM_EMIT3 (66, 89, 15)


#pragma pack(1)

struct Registers
{
	uint16 Flags;

	union
	{
		uint32 EAX;
		struct { uint16 AX; uint16 EAXH; };
	};

	union
	{
		uint32 EBX;
		struct { uint16 BX; uint16 EBXH; };
	};

	union
	{
		uint32 ECX;
		struct { uint16 CX; uint16 ECXH; };
	};

	union
	{
		uint32 EDX;
		struct { uint16 DX; uint16 EDXH; };
	};

	uint16 DI;
	uint16 SI;
	uint16 DS;
	uint16 ES;
	uint16 SS;
};

#pragma pack()


uint64 operator+ (const uint64 &a, const uint64 &b);
uint64 operator+ (const uint64 &a, uint32 b);
uint64 &operator+= (uint64 &a, const uint64 &b);
uint64 operator- (const uint64 &a, const uint64 &b);
uint64 operator- (const uint64 &a, uint32 b);
uint64 &operator-= (uint64 &a, const uint64 &b);
uint64 operator>> (const uint64 &a, int shiftCount);
uint64 operator<< (const uint64 &a, int shiftCount);
uint64 &operator++ (uint64 &a);
bool operator== (const uint64 &a, const uint64 &b);
bool operator> (const uint64 &a, const uint64 &b);
bool operator< (const uint64 &a, const uint64 &b);
bool operator>= (const uint64 &a, const uint64 &b);
bool operator<= (const uint64 &a, const uint64 &b);

void CopyMemory (void *source, uint16 destSegment, uint16 destOffset, uint16 blockSize);
void CopyMemory (uint16 sourceSegment, uint16 sourceOffset, void *destination, uint16 blockSize);
extern "C" void EraseMemory (void *memory, int size);
uint32 GetLinearAddress (uint16 segment, uint16 offset);
bool RegionsIntersect (const uint64 &start1, uint32 length1, const uint64 &start2, const uint64 &end2);
bool TestInt64 ();
extern "C" void ThrowFatalException (int line);

#endif // GST_HEADER_Boot_Platform
