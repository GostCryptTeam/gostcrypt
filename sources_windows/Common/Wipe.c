/**
 *
 *	\file Wipe.c
 *	\brief Manage the wiping functions in accordance to the advices given into some books about how to clearly erase data.
 *	\version 1.3.1
 *	\date 20/01/2017
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

#include "Gstdefs.h"
#include "Wipe.h"


/**
 *
 *	\fn static BOOL Wipe1PseudoRandom (int pass, byte *buffer, size_t size)
 *	\brief return FALSE
 *	\param[in] size_t size
 *	\param[in] byte * buffer
 *	\param[in] int pass
 *	\return BOOL FALSE
 *
 */
static BOOL Wipe1PseudoRandom (int pass, byte *buffer, size_t size)
{
	return FALSE;
}


// 

/**
 *
 *	\fn static BOOL Wipe3Dod5220 (int pass, byte *buffer, size_t size)
 *	\brief Fill buffer with wipe patterns defined in "National Industrial Security Program Operating Manual", US DoD 5220.22-M.
 *			Return: FALSE = buffer must be filled with random data
 *	\param[in] size_t size
 *	\param[in] byte * buffer
 *	\param[in] int pass
 *	\return BOOL TRUE
 *			switch (pass)
 *				case default FALSE
 *
 */
static BOOL Wipe3Dod5220 (int pass, byte *buffer, size_t size)
{
	byte wipeChar;

	switch (pass)
	{
	case 1:
		wipeChar = 0;
		break;

	case 2:
		wipeChar = 0xff;
		break;

	default:
		return FALSE;
	}

	memset (buffer, wipeChar, size);
	return TRUE;
}


/**
 *
 *	\fn static BOOL Wipe7Dod5220 (int pass, byte randChars[GST_WIPE_RAND_CHAR_COUNT], byte *buffer, size_t size)
 *	\brief Wipe.
 *	\param[in] size_t size
 *	\param[in] byte * buffer
 *	\param[in] byte randChars[GST_WIPE_RAND_CHAR_COUNT]
 *	\param[in] int pass
 *	\return BOOL switch (pass)
 *				case default FALSE
 			TRUE else
 *
 */
static BOOL Wipe7Dod5220 (int pass, byte randChars[GST_WIPE_RAND_CHAR_COUNT], byte *buffer, size_t size)
{
	byte wipeChar;

	switch (pass)
	{
	case 1:
		wipeChar = randChars[0];
		break;

	case 2:
		wipeChar = ~randChars[0];
		break;

	case 4:
		wipeChar = randChars[1];
		break;

	case 5:
		wipeChar = randChars[2];
		break;

	case 6:
		wipeChar = ~randChars[2];
		break;

	default:
		return FALSE;
	}

	memset (buffer, wipeChar, size);
	return TRUE;
}


/**
 *
 *	\fn static BOOL Wipe35Gutmann (int pass, byte *buffer, size_t size)
 *	\brief Fill the buffer with wipe patterns defined in the paper "Secure Deletion of Data from Magnetic and Solid-State Memory" by Peter Gutmann.
 *			Return: FALSE = buffer must be filled with random data
 *	\param[in] size_t size
 *	\param[in] byte * buffer
 *	\param[in] int pass
 *	\return BOOL FALSE else (pass >= 10 && pass <= 25)
 *			TRUE else
 *
 */
static BOOL Wipe35Gutmann (int pass, byte *buffer, size_t size)
{
	byte wipePat3[] = { 0x92, 0x49, 0x24 };
	int wipePat3Pos;
	size_t i;

	switch (pass)
	{
	case 5:
		memset (buffer, 0x55, size);
		break;

	case 6:
		memset (buffer, 0xaa, size);
		break;

	case 7:
	case 26:
	case 29:
		wipePat3Pos = 0;
		goto wipe3;

	case 8:
	case 27:
	case 30:
		wipePat3Pos = 1;
		goto wipe3;

	case 9:
	case 28:
	case 31:
		wipePat3Pos = 2;
		goto wipe3;

wipe3:
		if (pass >= 29)
		{
			wipePat3[0] = ~wipePat3[0];
			wipePat3[1] = ~wipePat3[1];
			wipePat3[2] = ~wipePat3[2];
		}

		for (i = 0; i < size; ++i)
		{
			buffer[i] = wipePat3[wipePat3Pos++ % 3];
		}
		break;

	default:
		if (pass >= 10 && pass <= 25)
			memset (buffer, (pass - 10) * 0x11, size);
		else
			return FALSE;
	}

	return TRUE;
}


/**
 *
 *	\fn int GetWipePassCount (WipeAlgorithmId algorithm)
 *	\brief Get the number of time required to wipe
 *	\param[in] WipeAlgorithmId algorithm
 *	\return int switch (algorithm)
 *				1 case GST_WIPE_1_RAND
 *				3 case GST_WIPE_3_DOD_5220
 *				7 case GST_WIPE_7_DOD_5220
 *				35 case GST_WIPE_35_GUTMANN
 *			0 else
 *
 */
int GetWipePassCount (WipeAlgorithmId algorithm)
{
	switch (algorithm)
	{
	case GST_WIPE_1_RAND:
		return 1;

	case GST_WIPE_3_DOD_5220:
		return 3;

	case GST_WIPE_7_DOD_5220:
		return 7;

	case GST_WIPE_35_GUTMANN:
		return 35;

	default:
		GST_THROW_FATAL_EXCEPTION;
	}

	return 0;	// Prevent compiler warnings
}


/**
 *
 *	\fn BOOL WipeBuffer (WipeAlgorithmId algorithm, byte randChars[GST_WIPE_RAND_CHAR_COUNT], int pass, byte *buffer, size_t size)
 *	\brief Wipe the buffer
 *	\param[in] size_t size
 *	\param[in] byte * buffer
 *	\param[in] int pass
 *	\param[in] byte randChars[GST_WIPE_RAND_CHAR_COUNT]
 *	\param[in] WipeAlgorithmId algorithm
 *	\return BOOL switch (algorithm)
 *				case GST_WIPE_1_RAND Wipe1PseudoRandom (pass, buffer, size)
 *				case GST_WIPE_3_DOD_5220 Wipe3Dod5220 (pass, buffer, size)
 *				case GST_WIPE_7_DOD_5220 Wipe7Dod5220 (pass, randChars, buffer, size)
 *				case GST_WIPE_35_GUTMANN Wipe35Gutmann (pass, buffer, size)
 *			FALSE else
 *
 */
BOOL WipeBuffer (WipeAlgorithmId algorithm, byte randChars[GST_WIPE_RAND_CHAR_COUNT], int pass, byte *buffer, size_t size)
{
	switch (algorithm)
	{
	case GST_WIPE_1_RAND:
		return Wipe1PseudoRandom (pass, buffer, size);

	case GST_WIPE_3_DOD_5220:
		return Wipe3Dod5220 (pass, buffer, size);

	case GST_WIPE_7_DOD_5220:
		return Wipe7Dod5220 (pass, randChars, buffer, size);

	case GST_WIPE_35_GUTMANN:
		return Wipe35Gutmann (pass, buffer, size);

	default:
		GST_THROW_FATAL_EXCEPTION;
	}

	return FALSE;	// Prevent compiler warnings
}
