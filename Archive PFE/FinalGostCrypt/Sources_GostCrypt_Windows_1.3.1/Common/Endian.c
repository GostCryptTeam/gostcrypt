/**
*
*	\file Endian.c
*	\brief Mirror the bytes of ints and reverse long
*	\version 1.3.1
*	\date 2009
*
*/

/**
 *	Legal Notice: Some portions of the source code contained in this file were
 *	derived from the source code of Encryption for the Masses 2.02a, which is
 *	Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 *	Agreement for Encryption for the Masses'. Modifications and additions to
 *	the original source code (contained in this file) and all other portions
 *	of this file are Copyright (c) 2003-2009 TrueCrypt Developers Association
 *	and are governed by the TrueCrypt License 3.0 the full text of which is
 *	contained in the file License.txt included in TrueCrypt binary and source
 *	code distribution packages.
 */


#include "Gstdefs.h"
#include "Common/Endian.h"


/**
 *
 *	\fn unsigned __int16 MirrorBytes16 (unsigned __int16 x)
 *	\brief Mirror the bytes of the __int16
 *	\param[in] unsigned __int16 x
 *	\return unsigned __int16 (x << 8) | (x >> 8)
 *
 */
unsigned __int16 MirrorBytes16 (unsigned __int16 x)
{
	return (x << 8) | (x >> 8);
}


/**
 *
 *	\fn unsigned __int32 MirrorBytes32 (unsigned __int32 x)
 *	\brief Mirror the bytes of the __int32
 *	\param[in] unsigned __int32 x
 *	\return unsigned __int32 (n << 8) | (unsigned __int8) (x >> 24)
 *
 */
unsigned __int32 MirrorBytes32 (unsigned __int32 x)
{
	unsigned __int32 n = (unsigned __int8) x;
	n <<= 8; n |= (unsigned __int8) (x >> 8);
	n <<= 8; n |= (unsigned __int8) (x >> 16);
	return (n << 8) | (unsigned __int8) (x >> 24);
}

#ifndef GST_NO_COMPILER_INT64
/**
 *
 *	\fn uint64 MirrorBytes64 (uint64 x)
 *	\brief Mirror the bytes of the __int64
 *	\param[in] uint64 x
 *	\return uint64 (n << 8) | (unsigned __int8) (x >> 56)
 *
 */
uint64 MirrorBytes64 (uint64 x)
{
	uint64 n = (unsigned __int8) x;
	n <<= 8; n |= (unsigned __int8) (x >> 8);
	n <<= 8; n |= (unsigned __int8) (x >> 16);
	n <<= 8; n |= (unsigned __int8) (x >> 24);
	n <<= 8; n |= (unsigned __int8) (x >> 32);
	n <<= 8; n |= (unsigned __int8) (x >> 40);
	n <<= 8; n |= (unsigned __int8) (x >> 48);
	return (n << 8) | (unsigned __int8) (x >> 56);
}
#endif

/**
 *
 *	\fn void LongReverse (unsigned __int32 *buffer, unsigned byteCount)
 *	\brief Reverse the bytes of a long
 *	\param[in] unsigned byteCount
 *	\param[in out] unsigned __int32 * buffer
 *	\return void
 *
 */
void LongReverse (unsigned __int32 *buffer, unsigned byteCount)
{
	unsigned __int32 value;

	byteCount /= sizeof (unsigned __int32);
	while (byteCount--)
	{
		value = *buffer;
		value = ((value & 0xFF00FF00L) >> 8) | \
		    ((value & 0x00FF00FFL) << 8);
		*buffer++ = (value << 16) | (value >> 16);
	}
}
