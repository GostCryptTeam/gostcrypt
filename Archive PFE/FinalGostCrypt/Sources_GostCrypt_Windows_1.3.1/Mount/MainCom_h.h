

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Jan 25 13:52:52 2017
 */
/* Compiler settings for MainCom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __MainCom_h_h__
#define __MainCom_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGostCryptMainCom_FWD_DEFINED__
#define __IGostCryptMainCom_FWD_DEFINED__
typedef interface IGostCryptMainCom IGostCryptMainCom;

#endif 	/* __IGostCryptMainCom_FWD_DEFINED__ */


#ifndef __GostCryptMainCom_FWD_DEFINED__
#define __GostCryptMainCom_FWD_DEFINED__

#ifdef __cplusplus
typedef class GostCryptMainCom GostCryptMainCom;
#else
typedef struct GostCryptMainCom GostCryptMainCom;
#endif /* __cplusplus */

#endif 	/* __GostCryptMainCom_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "Password.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __GostCryptMainCom_LIBRARY_DEFINED__
#define __GostCryptMainCom_LIBRARY_DEFINED__

/* library GostCryptMainCom */
/* [version][helpstring][uuid] */ 


EXTERN_C const IID LIBID_GostCryptMainCom;

#ifndef __IGostCryptMainCom_INTERFACE_DEFINED__
#define __IGostCryptMainCom_INTERFACE_DEFINED__

/* interface IGostCryptMainCom */
/* [helpstring][oleautomation][object][uuid] */ 


EXTERN_C const IID IID_IGostCryptMainCom;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("252C9DE6-D4B9-4A59-8A10-9CA73217B3D0")
    IGostCryptMainCom : public IUnknown
    {
    public:
        virtual void STDMETHODCALLTYPE AnalyzeKernelMiniDump( 
            LONG_PTR hwndDlg) = 0;
        
        virtual int STDMETHODCALLTYPE BackupVolumeHeader( 
            LONG_PTR hwndDlg,
            BOOL bRequireConfirmation,
            BSTR lpszVolume) = 0;
        
        virtual DWORD STDMETHODCALLTYPE CallDriver( 
            DWORD ioctl,
            BSTR input,
            BSTR *output) = 0;
        
        virtual int STDMETHODCALLTYPE ChangePassword( 
            BSTR volumePath,
            Password *oldPassword,
            Password *newPassword,
            int pkcs5,
            LONG_PTR hWnd) = 0;
        
        virtual DWORD STDMETHODCALLTYPE CopyFile( 
            BSTR sourceFile,
            BSTR destinationFile) = 0;
        
        virtual DWORD STDMETHODCALLTYPE DeleteFile( 
            BSTR file) = 0;
        
        virtual BOOL STDMETHODCALLTYPE IsPagingFileActive( 
            BOOL checkNonWindowsPartitionsOnly) = 0;
        
        virtual DWORD STDMETHODCALLTYPE ReadWriteFile( 
            BOOL write,
            BOOL device,
            BSTR filePath,
            BSTR *bufferBstr,
            unsigned __int64 offset,
            unsigned __int32 size,
            DWORD *sizeDone) = 0;
        
        virtual DWORD STDMETHODCALLTYPE RegisterFilterDriver( 
            BOOL registerDriver,
            int filterType) = 0;
        
        virtual DWORD STDMETHODCALLTYPE RegisterSystemFavoritesService( 
            BOOL registerService) = 0;
        
        virtual int STDMETHODCALLTYPE RestoreVolumeHeader( 
            LONG_PTR hwndDlg,
            BSTR lpszVolume) = 0;
        
        virtual DWORD STDMETHODCALLTYPE SetDriverServiceStartType( 
            DWORD startType) = 0;
        
        virtual DWORD STDMETHODCALLTYPE WriteLocalMachineRegistryDwordValue( 
            BSTR keyPath,
            BSTR valueName,
            DWORD value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGostCryptMainComVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGostCryptMainCom * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGostCryptMainCom * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGostCryptMainCom * This);
        
        void ( STDMETHODCALLTYPE *AnalyzeKernelMiniDump )( 
            IGostCryptMainCom * This,
            LONG_PTR hwndDlg);
        
        int ( STDMETHODCALLTYPE *BackupVolumeHeader )( 
            IGostCryptMainCom * This,
            LONG_PTR hwndDlg,
            BOOL bRequireConfirmation,
            BSTR lpszVolume);
        
        DWORD ( STDMETHODCALLTYPE *CallDriver )( 
            IGostCryptMainCom * This,
            DWORD ioctl,
            BSTR input,
            BSTR *output);
        
        int ( STDMETHODCALLTYPE *ChangePassword )( 
            IGostCryptMainCom * This,
            BSTR volumePath,
            Password *oldPassword,
            Password *newPassword,
            int pkcs5,
            LONG_PTR hWnd);
        
        DWORD ( STDMETHODCALLTYPE *CopyFile )( 
            IGostCryptMainCom * This,
            BSTR sourceFile,
            BSTR destinationFile);
        
        DWORD ( STDMETHODCALLTYPE *DeleteFile )( 
            IGostCryptMainCom * This,
            BSTR file);
        
        BOOL ( STDMETHODCALLTYPE *IsPagingFileActive )( 
            IGostCryptMainCom * This,
            BOOL checkNonWindowsPartitionsOnly);
        
        DWORD ( STDMETHODCALLTYPE *ReadWriteFile )( 
            IGostCryptMainCom * This,
            BOOL write,
            BOOL device,
            BSTR filePath,
            BSTR *bufferBstr,
            unsigned __int64 offset,
            unsigned __int32 size,
            DWORD *sizeDone);
        
        DWORD ( STDMETHODCALLTYPE *RegisterFilterDriver )( 
            IGostCryptMainCom * This,
            BOOL registerDriver,
            int filterType);
        
        DWORD ( STDMETHODCALLTYPE *RegisterSystemFavoritesService )( 
            IGostCryptMainCom * This,
            BOOL registerService);
        
        int ( STDMETHODCALLTYPE *RestoreVolumeHeader )( 
            IGostCryptMainCom * This,
            LONG_PTR hwndDlg,
            BSTR lpszVolume);
        
        DWORD ( STDMETHODCALLTYPE *SetDriverServiceStartType )( 
            IGostCryptMainCom * This,
            DWORD startType);
        
        DWORD ( STDMETHODCALLTYPE *WriteLocalMachineRegistryDwordValue )( 
            IGostCryptMainCom * This,
            BSTR keyPath,
            BSTR valueName,
            DWORD value);
        
        END_INTERFACE
    } IGostCryptMainComVtbl;

    interface IGostCryptMainCom
    {
        CONST_VTBL struct IGostCryptMainComVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGostCryptMainCom_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGostCryptMainCom_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGostCryptMainCom_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGostCryptMainCom_AnalyzeKernelMiniDump(This,hwndDlg)	\
    ( (This)->lpVtbl -> AnalyzeKernelMiniDump(This,hwndDlg) ) 

#define IGostCryptMainCom_BackupVolumeHeader(This,hwndDlg,bRequireConfirmation,lpszVolume)	\
    ( (This)->lpVtbl -> BackupVolumeHeader(This,hwndDlg,bRequireConfirmation,lpszVolume) ) 

#define IGostCryptMainCom_CallDriver(This,ioctl,input,output)	\
    ( (This)->lpVtbl -> CallDriver(This,ioctl,input,output) ) 

#define IGostCryptMainCom_ChangePassword(This,volumePath,oldPassword,newPassword,pkcs5,hWnd)	\
    ( (This)->lpVtbl -> ChangePassword(This,volumePath,oldPassword,newPassword,pkcs5,hWnd) ) 

#define IGostCryptMainCom_CopyFile(This,sourceFile,destinationFile)	\
    ( (This)->lpVtbl -> CopyFile(This,sourceFile,destinationFile) ) 

#define IGostCryptMainCom_DeleteFile(This,file)	\
    ( (This)->lpVtbl -> DeleteFile(This,file) ) 

#define IGostCryptMainCom_IsPagingFileActive(This,checkNonWindowsPartitionsOnly)	\
    ( (This)->lpVtbl -> IsPagingFileActive(This,checkNonWindowsPartitionsOnly) ) 

#define IGostCryptMainCom_ReadWriteFile(This,write,device,filePath,bufferBstr,offset,size,sizeDone)	\
    ( (This)->lpVtbl -> ReadWriteFile(This,write,device,filePath,bufferBstr,offset,size,sizeDone) ) 

#define IGostCryptMainCom_RegisterFilterDriver(This,registerDriver,filterType)	\
    ( (This)->lpVtbl -> RegisterFilterDriver(This,registerDriver,filterType) ) 

#define IGostCryptMainCom_RegisterSystemFavoritesService(This,registerService)	\
    ( (This)->lpVtbl -> RegisterSystemFavoritesService(This,registerService) ) 

#define IGostCryptMainCom_RestoreVolumeHeader(This,hwndDlg,lpszVolume)	\
    ( (This)->lpVtbl -> RestoreVolumeHeader(This,hwndDlg,lpszVolume) ) 

#define IGostCryptMainCom_SetDriverServiceStartType(This,startType)	\
    ( (This)->lpVtbl -> SetDriverServiceStartType(This,startType) ) 

#define IGostCryptMainCom_WriteLocalMachineRegistryDwordValue(This,keyPath,valueName,value)	\
    ( (This)->lpVtbl -> WriteLocalMachineRegistryDwordValue(This,keyPath,valueName,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGostCryptMainCom_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GostCryptMainCom;

#ifdef __cplusplus

class DECLSPEC_UUID("CECBC0EE-78D9-41E6-BCF1-BC222BB224BA")
GostCryptMainCom;
#endif
#endif /* __GostCryptMainCom_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


