/**
 *
 *	\file Combo.c
 *	\brief Manage functions on combo box
 *	\version 1.3.1
 *	\date 20/01/2017
 *
 */

/**
 *
 *	Legal Notice: Some portions of the source code contained in this file were
 *	derived from the source code of Encryption for the Masses 2.02a, which is
 *	Copyright (c) 1998-2000 Paul Le Roux and which is governed by the 'License
 *	Agreement for Encryption for the Masses'. Modifications and additions to
 *	the original source code (contained in this file) and all other portions
 *	of this file are Copyright (c) 2003-2008 TrueCrypt Developers Association
 *	and are governed by the TrueCrypt License 3.0 the full text of which is
 *	contained in the file License.txt included in TrueCrypt binary and source
 *	code distribution packages. 
 *
 */

#include "Gstdefs.h"
#include "Combo.h"
#include "Dlgcode.h"
#include "Xml.h"

#include <time.h>

#define SIZEOF_MRU_LIST 20

/**
 *
 *	\fn void AddComboItem (HWND hComboBox, char *lpszFileName, BOOL saveHistory)
 *	\brief Add item to combo box
 *	\param[in] BOOL saveHistory
 *	\param[in] char * lpszFileName
 *	\param[in] HWND hComboBox
 *	\return void
 *
 */
void AddComboItem (HWND hComboBox, char *lpszFileName, BOOL saveHistory)
{
	LPARAM nIndex;

	if (!saveHistory)
	{
		SendMessage (hComboBox, CB_RESETCONTENT, 0, 0);
		SetWindowText (hComboBox, lpszFileName);
		return;
	}

	nIndex = SendMessage (hComboBox, CB_FINDSTRINGEXACT, (WPARAM) - 1, (LPARAM) & lpszFileName[0]);

	if (nIndex == CB_ERR && *lpszFileName)
	{
		time_t lTime = time (NULL);
		nIndex = SendMessage (hComboBox, CB_ADDSTRING, 0, (LPARAM) & lpszFileName[0]);
		if (nIndex != CB_ERR)
			SendMessage (hComboBox, CB_SETITEMDATA, nIndex, (LPARAM) lTime);
	}

	if (nIndex != CB_ERR && *lpszFileName)
		nIndex = SendMessage (hComboBox, CB_SETCURSEL, nIndex, 0);

	if (*lpszFileName == 0)
	{
		SendMessage (hComboBox, CB_SETCURSEL, (WPARAM) - 1, 0);
	}
}


/**
 *
 *	\fn LPARAM MoveEditToCombo (HWND hComboBox, BOOL saveHistory)
 *	\brief Set edited value in a combo box
 *	\param[in] BOOL saveHistory
 *	\param[in] HWND hComboBox
 *	\return LPARAM 0 if (!saveHistory)
 *			nIndex if (strlen (szTmp) > 0)
 *			SendMessage (hComboBox, CB_GETCURSEL, 0, 0) else
 *
 */
LPARAM MoveEditToCombo (HWND hComboBox, BOOL saveHistory)
{
	char szTmp[GST_MAX_PATH] = {0}; // max = 260

	if (!saveHistory)
	{
		GetWindowText (hComboBox, szTmp, sizeof (szTmp));
		SendMessage (hComboBox, CB_RESETCONTENT, 0, 0);
		SetWindowText (hComboBox, szTmp);
		return 0;
	}

	GetWindowText (hComboBox, szTmp, sizeof (szTmp));
	
	if (strlen (szTmp) > 0)
	{
		LPARAM nIndex = SendMessage (hComboBox, CB_FINDSTRINGEXACT, (WPARAM) - 1,
					     (LPARAM) & szTmp[0]);
		if (nIndex == CB_ERR)
		{
			time_t lTime = time (NULL);
			nIndex = SendMessage (hComboBox, CB_ADDSTRING, 0, (LPARAM) & szTmp[0]);
			if (nIndex != CB_ERR)
				SendMessage (hComboBox, CB_SETITEMDATA, nIndex, (DWORD) lTime);
		}
		else
		{
			time_t lTime = time (NULL);
			SendMessage (hComboBox, CB_SETITEMDATA, nIndex, (DWORD) lTime);
		}

		return nIndex;
	}

	return SendMessage (hComboBox, CB_GETCURSEL, 0, 0);
}

/**
 *
 *	\fn int GetOrderComboIdx (HWND hComboBox, int *nIdxList, int nElems)
 *	\brief Get the combo highest id or return err if failed
 *	\param[in] int nElems
 *	\param[in] int * nIdxList
 *	\param[in] HWND hComboBox
 *	\return int nHighIdx if (x != CB_ERR)
 *			CB_ERR else
 *
 */
int GetOrderComboIdx (HWND hComboBox, int *nIdxList, int nElems)
{
	int x = (int) SendMessage (hComboBox, CB_GETCOUNT, 0, 0);
	if (x != CB_ERR)
	{
		int i, nHighIdx = CB_ERR;
		time_t lHighTime = -1;

		for (i = 0; i < x; i++)
		{
			time_t lTime = SendMessage (hComboBox, CB_GETITEMDATA, (WPARAM) i, 0);
			if (lTime > lHighTime)
			{
				int n;
				for (n = 0; n < nElems; n++)
					if (nIdxList[n] == i)
						break;
				if (n == nElems)
				{
					lHighTime = lTime;
					nHighIdx = i;
				}
			}
		}

		return nHighIdx;
	}

	return CB_ERR;
}

/**
 *
 *	\fn LPARAM UpdateComboOrder (HWND hComboBox)
 *	\brief Update the combo order
 *	\param[in] HWND hComboBox
 *	\return LPARAM nIndex
 *
 */
LPARAM UpdateComboOrder (HWND hComboBox)
{
	LPARAM nIndex;

	nIndex = SendMessage (hComboBox, CB_GETCURSEL, 0, 0);

	if (nIndex != CB_ERR)
	{
		time_t lTime = time (NULL);
		nIndex = SendMessage (hComboBox, CB_SETITEMDATA, (WPARAM) nIndex,
				      (LPARAM) lTime);
	}

	return nIndex;
}

/**
 *
 *	\fn void LoadCombo (HWND hComboBox)
 *	\brief Load a combo box
 *	\param[in] HWND hComboBox
 *	\return void
 *
 */
void LoadCombo (HWND hComboBox)
{
	DWORD size;
	char *history = LoadFile (GetConfigPath (GST_APPD_FILENAME_HISTORY), &size);
	char *xml = history;
	char volume[MAX_PATH];

	if (xml == NULL) return;

	while (xml = XmlFindElement (xml, "volume"))
	{
		XmlGetNodeText (xml, volume, sizeof (volume));
		AddComboItem (hComboBox, volume, TRUE);
		xml++;
	}

	SendMessage (hComboBox, CB_SETCURSEL, 0, 0);

	free (history);
}

/**
 *
 *	\fn void DumpCombo (HWND hComboBox, int bClear)
 *	\brief Dump a combo box in file as XML
 *	\param[in] int bClear
 *	\param[in] HWND hComboBox
 *	\return void
 *
 */
void DumpCombo (HWND hComboBox, int bClear)
{
	FILE *f;
	int i, nComboIdx[SIZEOF_MRU_LIST];

	if (bClear)
	{
		DeleteFile (GetConfigPath (GST_APPD_FILENAME_HISTORY));
		return;
	}

	f = fopen (GetConfigPath (GST_APPD_FILENAME_HISTORY), "w");
	if (f == NULL) return;

	XmlWriteHeader (f);
	fputs ("\n\t<history>", f);

	/* combo list part:- get mru items */
	for (i = 0; i < SIZEOF_MRU_LIST; i++)
		nComboIdx[i] = GetOrderComboIdx (hComboBox, &nComboIdx[0], i);

	/* combo list part:- write out mru items */
	for (i = 0; i < SIZEOF_MRU_LIST; i++)
	{
		char szTmp[MAX_PATH] = { 0 };
		 
		if (SendMessage (hComboBox, CB_GETLBTEXTLEN, nComboIdx[i], 0) < sizeof (szTmp))
			SendMessage (hComboBox, CB_GETLBTEXT, nComboIdx[i], (LPARAM) & szTmp[0]);

		if (szTmp[0] != 0)
		{
			char q[MAX_PATH * 2] = { 0 };
			XmlQuoteText (szTmp, q, sizeof (q));

			fprintf (f, "\n\t\t<volume>%s</volume>", q);
		}
	}

	fputs ("\n\t</history>", f);
	XmlWriteFooter (f);
	fclose (f);
}

/**
 *
 *	\fn void ClearCombo (HWND hComboBox)
 *	\brief Clear the combo box
 *	\param[in] HWND hComboBox
 *	\return void
 *
 */
void ClearCombo (HWND hComboBox)
{
	int i;
	for (i = 0; i < SIZEOF_MRU_LIST; i++)
	{
		SendMessage (hComboBox, CB_DELETESTRING, 0, 0);
	}
}

/**
 *
 *	\fn int IsComboEmpty (HWND hComboBox)
 *	\brief Is the combo box empty ?
 *	\param[in] HWND hComboBox
 *	\return int SendMessage (hComboBox, CB_GETCOUNT, 0, 0) < 1
 *
 */
int IsComboEmpty (HWND hComboBox)
{
	return SendMessage (hComboBox, CB_GETCOUNT, 0, 0) < 1;
}
