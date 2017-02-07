/**
 *
 *	\file MainCom.cpp
 *	\brief
 *	\version 1.3.1
 *	\date 14/12/2016
 *
 */

/*
 *	
 *	Copyright (c) 2007-2010 TrueCrypt Developers Association. All rights reserved.
 *	
 *	Governed by the TrueCrypt License 3.0 the full text of which is contained in
 *	the file License.txt included in TrueCrypt binary and source code distribution
 *	packages.
 *	
 */

#include <atlcomcli.h>
#include <atlconv.h>
#include <windows.h>
#include "BaseCom.h"
#include "BootEncryption.h"
#include "Dlgcode.h"
#include "MainCom.h"
#include "MainCom_h.h"
#include "MainCom_i.c"
#include "Mount.h"
#include "Password.h"

using namespace GostCrypt;

static volatile LONG ObjectCount = 0;

class GostCryptMainCom : public IGostCryptMainCom
{

public:
	GostCryptMainCom (DWORD messageThreadId) : RefCount (0), MessageThreadId (messageThreadId)
	{
		InterlockedIncrement (&ObjectCount);
	}

	~GostCryptMainCom ()
	{
		if (InterlockedDecrement (&ObjectCount) == 0)
			PostThreadMessage (MessageThreadId, WM_APP, 0, 0);
	}

	/**
	 *
	 *	\fn virtual ULONG STDMETHODCALLTYPE AddRef ()
	 *	\brief Add a reference
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
	 *	\return virtual ULONG 0 if (!InterlockedDecrement (&RefCount))
	 *			RefCount else
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
	 *	\brief Associate this object with the ppvObject if riid is unknow or mainCom
	 *	\param[in] void ** ppvObject
	 *	\param[in] REFIID riid
	 *	\return virtual HRESULT E_NOINTERFACE if !(riid == IID_IUnknown || riid == IID_IGostCryptMainCom)
	 *			S_OK else
	 *
	 */
	virtual HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void **ppvObject)
	{
		if (riid == IID_IUnknown || riid == IID_IGostCryptMainCom)
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
	 *	\fn virtual void STDMETHODCALLTYPE AnalyzeKernelMiniDump (LONG_PTR hwndDlg)
	 *	\brief Analyse the memory dump
	 *	\param[in] LONG_PTR hwndDlg
	 *	\return virtual void STDMETHODCALLTYPE
	 *
	 */
	virtual void STDMETHODCALLTYPE AnalyzeKernelMiniDump (LONG_PTR hwndDlg)
	{
		MainDlg = (HWND) hwndDlg;
		::AnalyzeKernelMiniDump ((HWND) hwndDlg);
	}

	/**
	 *
	 *	\fn virtual int STDMETHODCALLTYPE BackupVolumeHeader (LONG_PTR hwndDlg, BOOL bRequireConfirmation, BSTR lpszVolume)
	 *	\brief Create a backup of the volume header
	 *	\param[in] BSTR lpszVolume
	 *	\param[in] BOOL bRequireConfirmation
	 *	\param[in] LONG_PTR hwndDlg
	 *	\return virtual int STDMETHODCALLTYPE ::BackupVolumeHeader ((HWND) hwndDlg, bRequireConfirmation, CW2A (lpszVolume))
	 *
	 */
	virtual int STDMETHODCALLTYPE BackupVolumeHeader (LONG_PTR hwndDlg, BOOL bRequireConfirmation, BSTR lpszVolume)
	{
		USES_CONVERSION;
		MainDlg = (HWND) hwndDlg;
		return ::BackupVolumeHeader ((HWND) hwndDlg, bRequireConfirmation, CW2A (lpszVolume));
	}

	/**
	 *
	 *	\fn virtual int STDMETHODCALLTYPE RestoreVolumeHeader (LONG_PTR hwndDlg, BSTR lpszVolume)
	 *	\brief Restore the volume header
	 *	\param[in] BSTR lpszVolume
	 *	\param[in] LONG_PTR hwndDlg
	 *	\return virtual int STDMETHODCALLTYP ::RestoreVolumeHeader ((HWND) hwndDlg, CW2A (lpszVolume))
	 *
	 */
	virtual int STDMETHODCALLTYPE RestoreVolumeHeader (LONG_PTR hwndDlg, BSTR lpszVolume)
	{
		USES_CONVERSION;
		MainDlg = (HWND) hwndDlg;
		return ::RestoreVolumeHeader ((HWND) hwndDlg, CW2A (lpszVolume));
	}

	/**
	 *
	 *	\fn virtual DWORD STDMETHODCALLTYPE CallDriver (DWORD ioctl, BSTR input, BSTR *output)
	 *	\brief Call the "callDriver" of the Basecom object and return it. virtual and STDMETHODCALLTYPe handles event like mouse click ...
	 *	\param[in] BSTR * output
	 *	\param[in] BSTR input
	 *	\param[in] DWORD ioctl
	 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::CallDriver (ioctl, input, output
	 *
	 */
	virtual DWORD STDMETHODCALLTYPE CallDriver (DWORD ioctl, BSTR input, BSTR *output)
	{
		return BaseCom::CallDriver (ioctl, input, output);
	}

	/**
	 *
	 *	\fn virtual int STDMETHODCALLTYPE ChangePassword (BSTR volumePath, Password *oldPassword, Password *newPassword, int pkcs5, LONG_PTR hWnd)
	 *	\brief Change the password
	 *	\param[in] LONG_PTR hWnd
	 *	\param[in] int pkcs5
	 *	\param[in] Password * newPassword
	 *	\param[in] Password * oldPassword
	 *	\param[in] BSTR volumePath
	 *	\return virtual int STDMETHODCALLTYPE ::ChangePwd (CW2A (volumePath), oldPassword, newPassword, pkcs5, (HWND) hWnd)
	 *
	 */
	virtual int STDMETHODCALLTYPE ChangePassword (BSTR volumePath, Password *oldPassword, Password *newPassword, int pkcs5, LONG_PTR hWnd)
	{
		USES_CONVERSION;
		MainDlg = (HWND) hWnd;
		return ::ChangePwd (CW2A (volumePath), oldPassword, newPassword, pkcs5, (HWND) hWnd);
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
	 *	\return virtual DWORD STDMETHODCALLTYPE BaseCom::DeleteFile (file)
	 *
	 */
	virtual DWORD STDMETHODCALLTYPE DeleteFile (BSTR file)
	{
		return BaseCom::DeleteFile (file);
	}

	/**
	 *
	 *	\fn virtual BOOL STDMETHODCALLTYPE IsPagingFileActive (BOOL checkNonWindowsPartitionsOnly)
	 *	\brief Call the "IsPagingFileActive" of the Basecom object and return it
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
	 *	\fn virtual DWORD STDMETHODCALLTYPE ReadWriteFile (BOOL write, BOOL device, BSTR filePath, BSTR *bufferBstr, unsigned __int64 offset, unsigned __int32 size, DWORD *sizeDone)
	 *	\brief Call the "ReadWriteFile" of the Basecom object and return it
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
	 *	\brief Call the "RegisterFilterDriver" of the Basecom object and return it
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
	 *	\brief Call the "RegisterSystemFavoritesService" of the Basecom object and return it
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
	 *	\brief Call the "SetDriverServiceStartType" of the Basecom object and return it
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
	 *	\fn virtual DWORD STDMETHODCALLTYPE WriteLocalMachineRegistryDwordValue (BSTR keyPath, BSTR valueName, DWORD value)
	 *	\brief Call the "WriteLocalMachineRegistryDwordValue" of the Basecom object and return it
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
};


/**
 *
 *	\fn extern "C" BOOL ComServerMain ()
 *	\brief Dispatch message and revoke the class
 *	\return BOOL FALSE if (CoRegisterClassObject (CLSID_GostCryptMainCom, (LPUNKNOWN) &factory,CLSCTX_LOCAL_SERVER, REGCLS_SINGLEUSE, &cookie) != S_OK)
 *			FALSE if (r == -1)
 *			TRUE else
 *
 */
extern "C" BOOL ComServerMain ()
{
	SetProcessShutdownParameters (0x100, 0);

	GostCryptFactory<GostCryptMainCom> factory (GetCurrentThreadId ());
	DWORD cookie;

	if (IsUacSupported ())
		UacElevated = TRUE;

	if (CoRegisterClassObject (CLSID_GostCryptMainCom, (LPUNKNOWN) &factory,
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
 *	\fn static BOOL ComGetInstance (HWND hWnd, IGostCryptMainCom **gstServer)
 *	\brief Return a base instance
 *	\param[in] IGostCryptMainCom ** gstServer
 *	\param[in] HWND hWnd
 *	\return static BOOL ComGetInstanceBase (hWnd, CLSID_GostCryptMainCom, IID_IGostCryptMainCom, (void **) gstServer)
 *
 */
static BOOL ComGetInstance (HWND hWnd, IGostCryptMainCom **gstServer)
{
	return ComGetInstanceBase (hWnd, CLSID_GostCryptMainCom, IID_IGostCryptMainCom, (void **) gstServer);
}


/**
 *
 *	\fn IGostCryptMainCom *GetElevatedInstance (HWND parent)
 *	\brief Get the elevated instance (call ComGetInstance)
 *	\param[in] HWND parent
 *	\return IGostCryptMainCom * instance
 *
 */
IGostCryptMainCom *GetElevatedInstance (HWND parent)
{
	IGostCryptMainCom *instance;

	if (!ComGetInstance (parent, &instance))
		throw UserAbort (SRC_POS);

	return instance;
}


/**
 *
 *	\fn extern "C" void UacAnalyzeKernelMiniDump (HWND hwndDlg)
 *	\brief Analyse the uac memory dump
 *	\param[in] HWND hwndDlg
 *	\return void
 *
 */
extern "C" void UacAnalyzeKernelMiniDump (HWND hwndDlg)
{
	CComPtr<IGostCryptMainCom> gst;

	CoInitialize (NULL);

	if (ComGetInstance (hwndDlg, &gst))
	{
		WaitCursor();
		gst->AnalyzeKernelMiniDump ((LONG_PTR) hwndDlg);
		NormalCursor();
	}

	CoUninitialize ();
}


/**
 *
 *	\fn extern "C" int UacBackupVolumeHeader (HWND hwndDlg, BOOL bRequireConfirmation, char *lpszVolume)
 *	\brief Backup the uac volume header
 *	\param[in] char * lpszVolume
 *	\param[in] BOOL bRequireConfirmation
 *	\param[in] HWND hwndDlg
 *	\return int r
 *
 */
extern "C" int UacBackupVolumeHeader (HWND hwndDlg, BOOL bRequireConfirmation, char *lpszVolume)
{
	CComPtr<IGostCryptMainCom> gst;
	int r;

	CoInitialize (NULL);

	if (ComGetInstance (hwndDlg, &gst))
		r = gst->BackupVolumeHeader ((LONG_PTR) hwndDlg, bRequireConfirmation, CComBSTR (lpszVolume));
	else
		r = -1;

	CoUninitialize ();

	return r;
}


/**
 *
 *	\fn extern "C" int UacRestoreVolumeHeader (HWND hwndDlg, char *lpszVolume)
 *	\brief Restore the uac volume header
 *	\param[in] char * lpszVolume
 *	\param[in] HWND hwndDlg
 *	\return int r
 *
 */
extern "C" int UacRestoreVolumeHeader (HWND hwndDlg, char *lpszVolume)
{
	CComPtr<IGostCryptMainCom> gst;
	int r;

	CoInitialize (NULL);

	if (ComGetInstance (hwndDlg, &gst))
		r = gst->RestoreVolumeHeader ((LONG_PTR) hwndDlg, CComBSTR (lpszVolume));
	else
		r = -1;

	CoUninitialize ();

	return r;
}


/**
 *
 *	\fn extern "C" int UacChangePwd (char *lpszVolume, Password *oldPassword, Password *newPassword, int pkcs5, HWND hwndDlg)
 *	\brief Change uac password
 *	\param[in] HWND hwndDlg
 *	\param[in] int pkcs5
 *	\param[in] Password * newPassword
 *	\param[in] Password * oldPassword
 *	\param[in] char * lpszVolume
 *	\return int r
 *
 */
extern "C" int UacChangePwd (char *lpszVolume, Password *oldPassword, Password *newPassword, int pkcs5, HWND hwndDlg)
{
	CComPtr<IGostCryptMainCom> gst;
	int r;

	if (ComGetInstance (hwndDlg, &gst))
	{
		WaitCursor ();
		r = gst->ChangePassword (CComBSTR (lpszVolume), oldPassword, newPassword, pkcs5, (LONG_PTR) hwndDlg);
		NormalCursor ();
	}
	else
		r = -1;

	return r;
}
