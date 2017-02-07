

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Wed Jan 25 13:52:51 2017
 */
/* Compiler settings for FormatCom.idl:
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


#ifndef __FormatCom_h_h__
#define __FormatCom_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IGostCryptFormatCom_FWD_DEFINED__
#define __IGostCryptFormatCom_FWD_DEFINED__
typedef interface IGostCryptFormatCom IGostCryptFormatCom;

#endif 	/* __IGostCryptFormatCom_FWD_DEFINED__ */


#ifndef __GostCryptFormatCom_FWD_DEFINED__
#define __GostCryptFormatCom_FWD_DEFINED__

#ifdef __cplusplus
typedef class GostCryptFormatCom GostCryptFormatCom;
#else
typedef struct GostCryptFormatCom GostCryptFormatCom;
#endif /* __cplusplus */

#endif 	/* __GostCryptFormatCom_FWD_DEFINED__ */


/* header files for imported files */
#include "wtypes.h"
#include "Password.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __GostCryptFormatCom_LIBRARY_DEFINED__
#define __GostCryptFormatCom_LIBRARY_DEFINED__

/* library GostCryptFormatCom */
/* [version][helpstring][uuid] */ 


EXTERN_C const IID LIBID_GostCryptFormatCom;

#ifndef __IGostCryptFormatCom_INTERFACE_DEFINED__
#define __IGostCryptFormatCom_INTERFACE_DEFINED__

/* interface IGostCryptFormatCom */
/* [helpstring][oleautomation][object][uuid] */ 


EXTERN_C const IID IID_IGostCryptFormatCom;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9EE02955-174A-48F1-820C-022F327BE109")
    IGostCryptFormatCom : public IUnknown
    {
    public:
        virtual int STDMETHODCALLTYPE AnalyzeHiddenVolumeHost( 
            LONG_PTR hwndDlg,
            int *driveNo,
            __int64 hiddenVolHostSize,
            int *realClusterSize,
            __int64 *nbrFreeClusters) = 0;
        
        virtual DWORD STDMETHODCALLTYPE CallDriver( 
            DWORD ioctl,
            BSTR input,
            BSTR *output) = 0;
        
        virtual DWORD STDMETHODCALLTYPE CopyFile( 
            BSTR sourceFile,
            BSTR destinationFile) = 0;
        
        virtual DWORD STDMETHODCALLTYPE DeleteFile( 
            BSTR file) = 0;
        
        virtual BOOL STDMETHODCALLTYPE FormatNtfs( 
            int driveNo,
            int clusterSize) = 0;
        
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
        
        virtual DWORD STDMETHODCALLTYPE SetDriverServiceStartType( 
            DWORD startType) = 0;
        
        virtual DWORD STDMETHODCALLTYPE WriteLocalMachineRegistryDwordValue( 
            BSTR keyPath,
            BSTR valueName,
            DWORD value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IGostCryptFormatComVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IGostCryptFormatCom * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IGostCryptFormatCom * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IGostCryptFormatCom * This);
        
        int ( STDMETHODCALLTYPE *AnalyzeHiddenVolumeHost )( 
            IGostCryptFormatCom * This,
            LONG_PTR hwndDlg,
            int *driveNo,
            __int64 hiddenVolHostSize,
            int *realClusterSize,
            __int64 *nbrFreeClusters);
        
        DWORD ( STDMETHODCALLTYPE *CallDriver )( 
            IGostCryptFormatCom * This,
            DWORD ioctl,
            BSTR input,
            BSTR *output);
        
        DWORD ( STDMETHODCALLTYPE *CopyFile )( 
            IGostCryptFormatCom * This,
            BSTR sourceFile,
            BSTR destinationFile);
        
        DWORD ( STDMETHODCALLTYPE *DeleteFile )( 
            IGostCryptFormatCom * This,
            BSTR file);
        
        BOOL ( STDMETHODCALLTYPE *FormatNtfs )( 
            IGostCryptFormatCom * This,
            int driveNo,
            int clusterSize);
        
        BOOL ( STDMETHODCALLTYPE *IsPagingFileActive )( 
            IGostCryptFormatCom * This,
            BOOL checkNonWindowsPartitionsOnly);
        
        DWORD ( STDMETHODCALLTYPE *ReadWriteFile )( 
            IGostCryptFormatCom * This,
            BOOL write,
            BOOL device,
            BSTR filePath,
            BSTR *bufferBstr,
            unsigned __int64 offset,
            unsigned __int32 size,
            DWORD *sizeDone);
        
        DWORD ( STDMETHODCALLTYPE *RegisterFilterDriver )( 
            IGostCryptFormatCom * This,
            BOOL registerDriver,
            int filterType);
        
        DWORD ( STDMETHODCALLTYPE *RegisterSystemFavoritesService )( 
            IGostCryptFormatCom * This,
            BOOL registerService);
        
        DWORD ( STDMETHODCALLTYPE *SetDriverServiceStartType )( 
            IGostCryptFormatCom * This,
            DWORD startType);
        
        DWORD ( STDMETHODCALLTYPE *WriteLocalMachineRegistryDwordValue )( 
            IGostCryptFormatCom * This,
            BSTR keyPath,
            BSTR valueName,
            DWORD value);
        
        END_INTERFACE
    } IGostCryptFormatComVtbl;

    interface IGostCryptFormatCom
    {
        CONST_VTBL struct IGostCryptFormatComVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGostCryptFormatCom_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IGostCryptFormatCom_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IGostCryptFormatCom_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IGostCryptFormatCom_AnalyzeHiddenVolumeHost(This,hwndDlg,driveNo,hiddenVolHostSize,realClusterSize,nbrFreeClusters)	\
    ( (This)->lpVtbl -> AnalyzeHiddenVolumeHost(This,hwndDlg,driveNo,hiddenVolHostSize,realClusterSize,nbrFreeClusters) ) 

#define IGostCryptFormatCom_CallDriver(This,ioctl,input,output)	\
    ( (This)->lpVtbl -> CallDriver(This,ioctl,input,output) ) 

#define IGostCryptFormatCom_CopyFile(This,sourceFile,destinationFile)	\
    ( (This)->lpVtbl -> CopyFile(This,sourceFile,destinationFile) ) 

#define IGostCryptFormatCom_DeleteFile(This,file)	\
    ( (This)->lpVtbl -> DeleteFile(This,file) ) 

#define IGostCryptFormatCom_FormatNtfs(This,driveNo,clusterSize)	\
    ( (This)->lpVtbl -> FormatNtfs(This,driveNo,clusterSize) ) 

#define IGostCryptFormatCom_IsPagingFileActive(This,checkNonWindowsPartitionsOnly)	\
    ( (This)->lpVtbl -> IsPagingFileActive(This,checkNonWindowsPartitionsOnly) ) 

#define IGostCryptFormatCom_ReadWriteFile(This,write,device,filePath,bufferBstr,offset,size,sizeDone)	\
    ( (This)->lpVtbl -> ReadWriteFile(This,write,device,filePath,bufferBstr,offset,size,sizeDone) ) 

#define IGostCryptFormatCom_RegisterFilterDriver(This,registerDriver,filterType)	\
    ( (This)->lpVtbl -> RegisterFilterDriver(This,registerDriver,filterType) ) 

#define IGostCryptFormatCom_RegisterSystemFavoritesService(This,registerService)	\
    ( (This)->lpVtbl -> RegisterSystemFavoritesService(This,registerService) ) 

#define IGostCryptFormatCom_SetDriverServiceStartType(This,startType)	\
    ( (This)->lpVtbl -> SetDriverServiceStartType(This,startType) ) 

#define IGostCryptFormatCom_WriteLocalMachineRegistryDwordValue(This,keyPath,valueName,value)	\
    ( (This)->lpVtbl -> WriteLocalMachineRegistryDwordValue(This,keyPath,valueName,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IGostCryptFormatCom_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_GostCryptFormatCom;

#ifdef __cplusplus

class DECLSPEC_UUID("777DCDFD-C330-480B-B582-B02B57580CC9")
GostCryptFormatCom;
#endif
#endif /* __GostCryptFormatCom_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


