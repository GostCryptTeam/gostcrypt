/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#ifndef GST_HEADER_Boot_Bios
#define GST_HEADER_Boot_Bios

#include "Platform.h"

#define GST_LB_SIZE_BIT_SHIFT_DIVISOR 9

#define GST_FIRST_BIOS_DRIVE 0x80
#define GST_LAST_BIOS_DRIVE 0x8f
#define GST_INVALID_BIOS_DRIVE (GST_FIRST_BIOS_DRIVE - 1)

enum
{
	BiosResultSuccess = 0x00,
	BiosResultInvalidFunction = 0x01
};

typedef byte BiosResult;

#endif // GST_HEADER_Boot_Bios
