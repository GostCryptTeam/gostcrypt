
/*
 Copyright (c) 2008-2011 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/

#include <QtGlobal>

/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of Encryption for the Masses 2.02a, which is
 Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 Agreement for Encryption for the Masses'. */

#ifndef GSTDEFS_H
#define GSTDEFS_H

#define GST_APP_NAME						"GostCrypt"

// Version displayed to user 
#define VERSION_STRING					"1.3"

// Version number to compare against driver
#define VERSION_NUM						0x1300

// Release date
#define GST_STR_RELEASE_DATE				"September 1, 2015"
#define GST_RELEASE_DATE_YEAR			2015
#define GST_RELEASE_DATE_MONTH			9

#define BYTES_PER_KB                    1024LL
#define BYTES_PER_MB                    1048576LL
#define BYTES_PER_GB                    1073741824LL
#define BYTES_PER_TB                    1099511627776LL
#define BYTES_PER_PB                    1125899906842624LL

/* GUI/driver errors */

#define WIDE(x) (LPWSTR)L##x

#include <inttypes.h>
#include <limits.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#if UCHAR_MAX != 0xffU
#error UCHAR_MAX != 0xff
#endif
#define __int8 char

#if USHRT_MAX != 0xffffU
#error USHRT_MAX != 0xffff
#endif
#define __int16 short

#if UINT_MAX != 0xffffffffU
#error UINT_MAX != 0xffffffff
#endif
#define __int32 int


#define BOOL int
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif


#define GST_INT_TYPES_DEFINED

// Integer types required by Cryptolib
typedef unsigned __int8 uint_8t;
typedef unsigned __int16 uint_16t;
typedef unsigned __int32 uint_32t;


#define GST_THROW_FATAL_EXCEPTION	*(char *) 0 = 0

#define GSTalloc malloc
#define GSTfree free


#ifndef GST_TO_STRING
#	define GST_TO_STRING2(n) #n
#	define GST_TO_STRING(n) GST_TO_STRING2(n)
#endif

#define burn(mem,size) do { volatile char *burnm = (volatile char *)(mem); int burnc = size; while (burnc--) *burnm++ = 0; } while (0)

// The size of the memory area to wipe is in bytes amd it must be a multiple of 8.
#ifndef GST_NO_COMPILER_INT64
#	define FAST_ERASE64(mem,size) do { volatile quint64 *burnm = (volatile quint64 *)(mem); int burnc = size >> 3; while (burnc--) *burnm++ = 0; } while (0)
#else
#	define FAST_ERASE64(mem,size) do { volatile unsigned __int32 *burnm = (volatile unsigned __int32 *)(mem); int burnc = size >> 2; while (burnc--) *burnm++ = 0; } while (0)
#endif

#ifdef MAX_PATH
#define GST_MAX_PATH		MAX_PATH
#else
#define GST_MAX_PATH		260	/* Includes the null terminator */
#endif

#define GST_STR_RELEASED_BY "Released by GostCrypt Foundation on " GST_STR_RELEASE_DATE

#define MAX_URL_LENGTH	2084 /* Internet Explorer limit. Includes the terminating null character. */

#define GST_HOMEPAGE "http://www.gostcrypt.org/"
#define GST_APPLINK "http://www.gostcrypt.org/" 
#define GST_APPLINK_SECURE "https://www.gostcrypt.org/"

enum
{
	/* WARNING: ADD ANY NEW CODES AT THE END (DO NOT INSERT THEM BETWEEN EXISTING). DO *NOT* DELETE ANY 
	EXISTING CODES! Changing these values or their meanings may cause incompatibility with other versions
	(for example, if a new version of the GostCrypt installer receives an error code from an installed 
	driver whose version is lower, it will report and interpret the error incorrectly). */

	ERR_SUCCESS								= 0,
	ERR_OS_ERROR							= 1,
	ERR_OUTOFMEMORY							= 2,
	ERR_PASSWORD_WRONG						= 3,
	ERR_VOL_FORMAT_BAD						= 4,
	ERR_DRIVE_NOT_FOUND						= 5,
	ERR_FILES_OPEN							= 6,
	ERR_VOL_SIZE_WRONG						= 7,
	ERR_COMPRESSION_NOT_SUPPORTED			= 8,
	ERR_PASSWORD_CHANGE_VOL_TYPE			= 9,
	ERR_PASSWORD_CHANGE_VOL_VERSION			= 10,
	ERR_VOL_SEEKING							= 11,
	ERR_VOL_WRITING							= 12,
	ERR_FILES_OPEN_LOCK						= 13,
	ERR_VOL_READING							= 14,
	ERR_DRIVER_VERSION						= 15,
	ERR_NEW_VERSION_REQUIRED				= 16,
	ERR_CIPHER_INIT_FAILURE					= 17,
	ERR_CIPHER_INIT_WEAK_KEY				= 18,
	ERR_SELF_TESTS_FAILED					= 19,
	ERR_SECTOR_SIZE_INCOMPATIBLE			= 20,
	ERR_VOL_ALREADY_MOUNTED					= 21,
	ERR_NO_FREE_DRIVES						= 22,
	ERR_FILE_OPEN_FAILED					= 23,
	ERR_VOL_MOUNT_FAILED					= 24,
	DEPRECATED_ERR_INVALID_DEVICE			= 25,
	ERR_ACCESS_DENIED						= 26,
	ERR_MODE_INIT_FAILED					= 27,
	ERR_DONT_REPORT							= 28,
	ERR_ENCRYPTION_NOT_COMPLETED			= 29,
	ERR_PARAMETER_INCORRECT					= 30,
	ERR_SYS_HIDVOL_HEAD_REENC_MODE_WRONG	= 31,
	ERR_NONSYS_INPLACE_ENC_INCOMPLETE		= 32,
	ERR_USER_ABORT							= 33
};

#endif 	// #ifndef GSTDEFS_H
