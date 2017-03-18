/**
*
*	\file FormatCom.cpp
*	\brief GostCryptFormatCom Class
*	\version 1.3.1
*	\date 14/12/2016
*
*/

/**
 *	Copyright (c) 2007-2008 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 */

#include <atlcomcli.h>
#include <atlconv.h>
#include <comutil.h>
#include <windows.h>
#include "BaseCom.h"
#include "BootEncryption.h"
#include "Dlgcode.h"
#include "Format.h"
#include "Progress.h"
#include "GstFormat.h"
#include "FormatCom.h"
#include "FormatCom_h.h"
#include "FormatCom_i.c"

using namespace GostCrypt;

static volatile LONG ObjectCount = 0;

class GostCryptFormatCom : public IGostCryptFormatCom
{

public:
	GostCryptFormatCom (DWORD messageThreadId) : RefCount (0),
		MessageThreadId (messageThreadId),
		CallBack (NULL)
	{
		InterlockedIncrement (&ObjectCount);
	}

	~GostCryptFormatCom ()
	{
		if (InterlockedDecrement (&ObjectCount) == 0)
			PostThreadMessage (MessageThreadId, WM_APP, 0, 0);
	}

/**
 *
 *	\fn virtual ULONG STDMETHODCALLTYPE AddRef ()
 *	\brief Add a reference: see https://msdn.microsoft.com/fr-fr/library/dd78zt0c(v=vs.110).aspx
 *	\return virtual ULONG InterlockedIncrement (&RefCount)
 *
 */
	virtual ULONG STDMETHODCALLTYPE AddRef ()
	{
		return InterlockedIncrement (&RefCount);
	}

/**
 *
 *	\fn virtual ULONG STDMETHODCALLTYPE Release ()
 *	\brief Release a reference
 *	\return virtual ULONG RefCount
 *
 */
	virtual ULONG STDMETHODCALLTYPE Release ()
	{
		if (!InterlockedDecrement (&RefCount))
		{
			delete this;
			return 0;
		}

		return RefCount;
	}

/**
 *
 *	\fn virtual HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void **ppvObject)
 *	\brief Associate this object with the ppvObject if riid is unknow or formatCom
 *	\param[in] void ** ppvObject
 *	\param[in] REFIID riid
 *	\return virtual HRESULT STDMETHODCALLTYPE S_OK if ok
 *			E_NOINTERFACE otherwise
 *
 */
	virtual HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IGostCryptFormatCom)
			*ppvObject = this;
		else
		{
			*ppvObject = NULL;
			return E_NOINTERFACE;
		}

		AddRef ();
		return S_OK;
	}
	
/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE CallDriver (DWORD ioctl, BSTR input, BSTR *output)
 *	\brief Call the "callDriver" of the Basecom object and return it. virtual and STDMETHODCALLTYPe handles event like mouse click ...
 *	\param[in] BSTR * output
 *	\param[in] BSTR input
 *	\param[in] DWORD ioctl
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::CallDriver (ioctl, input, output)
 *
 */
	virtual DWORD STDMETHODCALLTYPE CallDriver (DWORD ioctl, BSTR input, BSTR *output)
	{
		return BaseCom::CallDriver (ioctl, input, output);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE CopyFile (BSTR sourceFile, BSTR destinationFile)
 *	\brief Call the "copyfile" of the Basecom object and return it
 *	\param[in] BSTR destinationFile
 *	\param[in] BSTR sourceFile
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::CopyFile (sourceFile, destinationFile)
 *
 */
	virtual DWORD STDMETHODCALLTYPE CopyFile (BSTR sourceFile, BSTR destinationFile)
	{
		return BaseCom::CopyFile (sourceFile, destinationFile);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE DeleteFile (BSTR file)
 *	\brief Call the "deletefile" of the Basecom object and return it
 *	\param[in] BSTR file
 *	\return virtual DWORD STDMETHODCALLTYPE DeleteFile BaseCom::DeleteFile (file)
 *
 */
	virtual DWORD STDMETHODCALLTYPE DeleteFile (BSTR file)
	{
		return BaseCom::DeleteFile (file);
	}

/**
 *
 *	\fn virtual BOOL STDMETHODCALLTYPE FormatNtfs (int driveNo, int clusterSize)
 *	\brief Call the format ntfs function in format.c 
 *	\param[in] int clusterSize
 *	\param[in] int driveNo
 *	\return virtual BOOL STDMETHODCALLTYPE ::FormatNtfs (driveNo, clusterSize)
 *
 */
	virtual BOOL STDMETHODCALLTYPE FormatNtfs (int driveNo, int clusterSize)
	{
		return ::FormatNtfs (driveNo, clusterSize);
	}

/**
 *
 *	\fn virtual int STDMETHODCALLTYPE AnalyzeHiddenVolumeHost (LONG_PTR hwndDlg, int *driveNo, __int64 hiddenVolHostSize, int *realClusterSize, __int64 *nbrFreeClusters)
 *	\brief Call the AnalyseHiddenVolumeHost in GstFormat.c
 *	\param[in] __int64 * nbrFreeClusters
 *	\param[in] int * realClusterSize
 *	\param[in] __int64 hiddenVolHostSize
 *	\param[in] int * driveNo
 *	\param[in] LONG_PTR hwndDlg
 *	\return virtual int STDMETHODCALLTYPE ::AnalyzeHiddenVolumeHost ((HWND) hwndDlg, driveNo, hiddenVolHostSize, realClusterSize, nbrFreeClusters)
 *
 */
	virtual int STDMETHODCALLTYPE AnalyzeHiddenVolumeHost (LONG_PTR hwndDlg, int *driveNo, __int64 hiddenVolHostSize, int *realClusterSize, __int64 *nbrFreeClusters)
	{
		return ::AnalyzeHiddenVolumeHost (
			(HWND) hwndDlg, driveNo, hiddenVolHostSize, realClusterSize, nbrFreeClusters);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE ReadWriteFile (BOOL write, BOOL device, BSTR filePath, BSTR *bufferBstr, unsigned __int64 offset, unsigned __int32 size, DWORD *sizeDone)
 *	\brief Call the ReadWriteFile function in basecom object
 *	\param[in] DWORD * sizeDone
 *	\param[in] unsigned __int32
 *	\param[in] unsigned __int64
 *	\param[in] BSTR * bufferBstr
 *	\param[in] BSTR filePath
 *	\param[in] BOOL device
 *	\param[in] BOOL write
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::ReadWriteFile (write, device, filePath, bufferBstr, offset, size, sizeDone)
 *
 */
	virtual DWORD STDMETHODCALLTYPE ReadWriteFile (BOOL write, BOOL device, BSTR filePath, BSTR *bufferBstr, unsigned __int64 offset, unsigned __int32 size, DWORD *sizeDone)
	{
		return BaseCom::ReadWriteFile (write, device, filePath, bufferBstr, offset, size, sizeDone);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE RegisterFilterDriver (BOOL registerDriver, int filterType)
 *	\brief Call RegisterFilterDriver in basecom object 
 *	\param[in] int filterType
 *	\param[in] BOOL registerDriver
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::RegisterFilterDriver (registerDriver, filterType)
 *
 */
	virtual DWORD STDMETHODCALLTYPE RegisterFilterDriver (BOOL registerDriver, int filterType)
	{
		return BaseCom::RegisterFilterDriver (registerDriver, filterType);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE RegisterSystemFavoritesService (BOOL registerService)
 *	\brief Call RegisterSystemFavoritesService in basecom object 
 *	\param[in] BOOL registerService
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::RegisterSystemFavoritesService (registerService)
 *
 */
	virtual DWORD STDMETHODCALLTYPE RegisterSystemFavoritesService (BOOL registerService)
	{
		return BaseCom::RegisterSystemFavoritesService (registerService);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE SetDriverServiceStartType (DWORD startType)
 *	\brief Call BaseCom SetDriverServiceStartType in basecom object 
 *	\param[in] DWORD startType
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::SetDriverServiceStartType (startType)
 *
 */
	virtual DWORD STDMETHODCALLTYPE SetDriverServiceStartType (DWORD startType)
	{
		return BaseCom::SetDriverServiceStartType (startType);
	}

/**
 *
 *	\fn virtual BOOL STDMETHODCALLTYPE IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
 *	\brief Call BaseCom IsPagingFileActive in basecom object 
 *	\param[in] BOOL checkNonWindowsPartitionsOnly
 *	\return virtual BOOL STDMETHODCALLTYPE BaseCom::IsPagingFileActive (checkNonWindowsPartitionsOnly)
 *
 */
	virtual BOOL STDMETHODCALLTYPE IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
	{
		return BaseCom::IsPagingFileActive (checkNonWindowsPartitionsOnly);
	}

/**
 *
 *	\fn virtual DWORD STDMETHODCALLTYPE WriteLocalMachineRegistryDwordValue (BSTR keyPath, BSTR valueName, DWORD value)
 *	\brief Call BaseCom WriteLocalMachineRegistryDwordValue in basecom object 
 *	\param[in] DWORD value
 *	\param[in] BSTR valueName
 *	\param[in] BSTR keyPath
 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::WriteLocalMachineRegistryDwordValue (keyPath, valueName, value)
 *
 */
	virtual DWORD STDMETHODCALLTYPE WriteLocalMachineRegistryDwordValue (BSTR keyPath, BSTR valueName, DWORD value)
	{
		return BaseCom::WriteLocalMachineRegistryDwordValue (keyPath, valueName, value);
	}

protected:
	DWORD MessageThreadId;
	LONG RefCount;
	IGostCryptFormatCom *CallBack;
};


/**
 *
 *	\fn extern "C" BOOL ComServerFormat ()
 *	\brief Dispatch message and revoke the class
 *	\return BOOL FALSE if (CoRegisterClassObject (CLSID_GostCryptFormatCom, (LPUNKNOWN) &factory,CLSCTX_LOCAL_SERVER, REGCLS_SINGLEUSE, &cookie) != S_OK)
 *	FALSE if (r == -1)
 *	TRUE otherwise
 *
 */
extern "C" BOOL ComServerFormat ()
{
	SetProcessShutdownParameters (0x100, 0);

	GostCryptFactory<GostCryptFormatCom> factory (GetCurrentThreadId ());
	DWORD cookie;

	if (IsUacSupported ())
		UacElevated = TRUE;

	if (CoRegisterClassObject (CLSID_GostCryptFormatCom, (LPUNKNOWN) &factory,
		CLSCTX_LOCAL_SERVER, REGCLS_SINGLEUSE, &cookie) != S_OK)
		return FALSE;

	MSG msg;
	while (int r = GetMessage (&msg, NULL, 0, 0))
	{
		if (r == -1)
			return FALSE;

		TranslateMessage (&msg);
		DispatchMessage (&msg);

		if (msg.message == WM_APP
			&& ObjectCount < 1
			&& !factory.IsServerLocked ())
			break;
	}
	CoRevokeClassObject (cookie);

	return TRUE;
}


/**
 *
 *	\fn static BOOL ComGetInstance (HWND hWnd, IGostCryptFormatCom **gstServer)
 *	\brief Return a base instance
 *	\param[in] IGostCryptFormatCom ** gstServer
 *	\param[in] HWND hWnd
 *	\return static BOOL ComGetInstanceBase (hWnd, CLSID_GostCryptFormatCom, IID_IGostCryptFormatCom, (void **) gstServer)
 *
 */
static BOOL ComGetInstance (HWND hWnd, IGostCryptFormatCom **gstServer)
{
	return ComGetInstanceBase (hWnd, CLSID_GostCryptFormatCom, IID_IGostCryptFormatCom, (void **) gstServer);
}


/**
 *
 *	\fn IGostCryptFormatCom *GetElevatedInstance (HWND parent)
 *	\brief Get the elevated instance (call ComGetInstance)
 *	\param[in] HWND parent
 *	\return IGostCryptFormatCom * instance
 *
 */
IGostCryptFormatCom *GetElevatedInstance (HWND parent)
{
	IGostCryptFormatCom *instance;

	if (!ComGetInstance (parent, &instance))
		throw UserAbort (SRC_POS);

	return instance;
}


/**
 *
 *	\fn extern "C" int UacFormatNtfs (HWND hWnd, int driveNo, int clusterSize)
 *	\brief Call formatNtfs to format
 *	\param[in] int clusterSize
 *	\param[in] int driveNo
 *	\param[in] HWND hWnd
 *	\return int r
 *
 */
extern "C" int UacFormatNtfs (HWND hWnd, int driveNo, int clusterSize)
{
	CComPtr<IGostCryptFormatCom> gst;
	int r;

	CoInitialize (NULL);

	if (ComGetInstance (hWnd, &gst))
		r = gst->FormatNtfs (driveNo, clusterSize);
	else
		r = 0;

	CoUninitialize ();

	return r;
}


/**
 *
 *	\fn extern "C" int UacAnalyzeHiddenVolumeHost (HWND hwndDlg, int *driveNo, __int64 hiddenVolHostSize, int *realClusterSize, __int64 *nbrFreeClusters)
 *	\brief Analyse hidden volume
 *	\param[in] __int64 * nbrFreeClusters
 *	\param[in] int * realClusterSize
 *	\param[in] __int64 hiddenVolHostSize
 *	\param[in] int * driveNo
 *	\param[in] HWND hwndDlg
 *	\return int r = gst->AnalyzeHiddenVolumeHost ((LONG_PTR) hwndDlg, driveNo, hiddenVolHostSize, realClusterSize, nbrFreeClusters) if(ComGetInstance (hwndDlg, &gst))
 *			0 otherwise
 *
 */
extern "C" int UacAnalyzeHiddenVolumeHost (HWND hwndDlg, int *driveNo, __int64 hiddenVolHostSize, int *realClusterSize, __int64 *nbrFreeClusters)
{
	CComPtr<IGostCryptFormatCom> gst;
	int r;

	CoInitialize (NULL);

	if (ComGetInstance (hwndDlg, &gst))
		r = gst->AnalyzeHiddenVolumeHost ((LONG_PTR) hwndDlg, driveNo, hiddenVolHostSize, realClusterSize, nbrFreeClusters);
	else
		r = 0;

	CoUninitialize ();

	return r;
}
