/**
 *
 *	\file ComSetup.cpp
 *	\brief Manage the registery for the program
 *	\version 1.3.1
 *	\date 14/12/2016
 *
 */

/**
 *	
 *	Copyright (c) 2007-2010 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#define GST_MAIN_COM_VERSION_MAJOR 2
#define GST_MAIN_COM_VERSION_MINOR 4

#define GST_FORMAT_COM_VERSION_MAJOR 2
#define GST_FORMAT_COM_VERSION_MINOR 4

#include <atlbase.h>
#include <comdef.h>
#include <statreg.h>
#include <windows.h>
#include "ComSetup.h"
#include "Dlgcode.h"
#include "Resource.h"
#include "../Mount/MainCom_i.c"
#include "../Format/FormatCom_i.c"


/**
 *
 *	\fn extern "C" BOOL RegisterComServers (char *modulePath)
 *	\brief Register GostCryptMainCom and GostCryptFormatCom, it seems calling them to create GostCrypt.exe and GostCrypt Format.exe in the registery
 *	\param[in] char * modulePath
 *	\return BOOL ret
 *
 */
extern "C" BOOL RegisterComServers (char *modulePath)
{
	BOOL ret = TRUE;
	wchar_t mainModule[1024], formatModule[1024];
	CComPtr<ITypeLib> tl, tl2;

	wsprintfW (mainModule, L"%hsGostCrypt.exe", modulePath);
	wsprintfW (formatModule, L"%hsGostCrypt Format.exe", modulePath);

	UnRegisterTypeLib (LIBID_GostCryptMainCom, GST_MAIN_COM_VERSION_MAJOR, GST_MAIN_COM_VERSION_MINOR, 0, SYS_WIN32);
	UnRegisterTypeLib (LIBID_GostCryptFormatCom, GST_FORMAT_COM_VERSION_MAJOR, GST_FORMAT_COM_VERSION_MINOR, 0, SYS_WIN32);

	wchar_t setupModule[MAX_PATH];
	GetModuleFileNameW (NULL, setupModule, sizeof (setupModule) / sizeof (setupModule[0]));

	CRegObject ro;
	HRESULT r;

	if (!SUCCEEDED (r = ro.FinalConstruct ())
		|| !SUCCEEDED (r = ro.AddReplacement (L"MAIN_MODULE", mainModule))
		|| !SUCCEEDED (r = ro.AddReplacement (L"FORMAT_MODULE", formatModule))
		|| !SUCCEEDED (r = ro.ResourceRegister (setupModule, IDR_COMREG, L"REGISTRY"))
		|| !SUCCEEDED (r = LoadTypeLib (mainModule, &tl))
		|| !SUCCEEDED (r = RegisterTypeLib (tl, mainModule, 0))
		|| !SUCCEEDED (r = LoadTypeLib (formatModule, &tl2))
		|| !SUCCEEDED (r = RegisterTypeLib (tl2, formatModule, 0)))
	{
		MessageBox (MainDlg, _com_error (r).ErrorMessage(), GST_APP_NAME, MB_ICONERROR);
		ret = FALSE;
	}

	ro.FinalRelease ();
	return ret;
}


/**
 *
 *	\fn extern "C" BOOL UnregisterComServers (char *modulePath)
 *	\brief Unregister GostCryptMainCom and GostCryptFormatCom, 
 *			it seems calling them to create GostCrypt.exe and GostCrypt Format.exe in the registery
 *	\param[in] char * modulePath
 *	\return BOOL ret
 *
 */
extern "C" BOOL UnregisterComServers (char *modulePath)
{
	BOOL ret;

	if (UnRegisterTypeLib (LIBID_GostCryptMainCom, GST_MAIN_COM_VERSION_MAJOR, GST_MAIN_COM_VERSION_MINOR, 0, SYS_WIN32) != S_OK)
		return FALSE;
	if (UnRegisterTypeLib (LIBID_GostCryptFormatCom, GST_FORMAT_COM_VERSION_MAJOR, GST_FORMAT_COM_VERSION_MINOR, 0, SYS_WIN32) != S_OK)
		return FALSE;

	wchar_t module[1024];
	CRegObject ro;
	ro.FinalConstruct ();

	wsprintfW (module, L"%hsGostCrypt.exe", modulePath);
	ro.AddReplacement (L"MAIN_MODULE", module);

	wsprintfW (module, L"%hsGostCrypt Format.exe", modulePath);
	ro.AddReplacement (L"FORMAT_MODULE", module);

	wchar_t setupModule[MAX_PATH];
	GetModuleFileNameW (NULL, setupModule, sizeof (setupModule) / sizeof (setupModule[0]));

	ret = ro.ResourceUnregister (setupModule, IDR_COMREG, L"REGISTRY") == S_OK;

	ro.FinalRelease ();
	return ret;
}