

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Mar 07 17:46:19 2022
 */
/* Compiler settings for PDF4Ax.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __PDF4Axidl_h__
#define __PDF4Axidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DPDF4Ax_FWD_DEFINED__
#define ___DPDF4Ax_FWD_DEFINED__
typedef interface _DPDF4Ax _DPDF4Ax;
#endif 	/* ___DPDF4Ax_FWD_DEFINED__ */


#ifndef ___DPDF4AxEvents_FWD_DEFINED__
#define ___DPDF4AxEvents_FWD_DEFINED__
typedef interface _DPDF4AxEvents _DPDF4AxEvents;
#endif 	/* ___DPDF4AxEvents_FWD_DEFINED__ */


#ifndef __PDF4Ax_FWD_DEFINED__
#define __PDF4Ax_FWD_DEFINED__

#ifdef __cplusplus
typedef class PDF4Ax PDF4Ax;
#else
typedef struct PDF4Ax PDF4Ax;
#endif /* __cplusplus */

#endif 	/* __PDF4Ax_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __PDF4AxLib_LIBRARY_DEFINED__
#define __PDF4AxLib_LIBRARY_DEFINED__

/* library PDF4AxLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_PDF4AxLib;

#ifndef ___DPDF4Ax_DISPINTERFACE_DEFINED__
#define ___DPDF4Ax_DISPINTERFACE_DEFINED__

/* dispinterface _DPDF4Ax */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DPDF4Ax;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("3FF92EA8-9206-48D0-90F2-00F8A9A98E16")
    _DPDF4Ax : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPDF4AxVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPDF4Ax * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPDF4Ax * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPDF4Ax * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPDF4Ax * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPDF4Ax * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPDF4Ax * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPDF4Ax * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DPDF4AxVtbl;

    interface _DPDF4Ax
    {
        CONST_VTBL struct _DPDF4AxVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPDF4Ax_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPDF4Ax_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPDF4Ax_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPDF4Ax_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPDF4Ax_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPDF4Ax_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPDF4Ax_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPDF4Ax_DISPINTERFACE_DEFINED__ */


#ifndef ___DPDF4AxEvents_DISPINTERFACE_DEFINED__
#define ___DPDF4AxEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DPDF4AxEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DPDF4AxEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("7D43C1E0-B4DF-4E6F-9E34-4DDBF5258906")
    _DPDF4AxEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DPDF4AxEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DPDF4AxEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DPDF4AxEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DPDF4AxEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DPDF4AxEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DPDF4AxEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DPDF4AxEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DPDF4AxEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DPDF4AxEventsVtbl;

    interface _DPDF4AxEvents
    {
        CONST_VTBL struct _DPDF4AxEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DPDF4AxEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DPDF4AxEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DPDF4AxEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DPDF4AxEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DPDF4AxEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DPDF4AxEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DPDF4AxEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DPDF4AxEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PDF4Ax;

#ifdef __cplusplus

class DECLSPEC_UUID("FE687896-F410-4D10-8740-D584DA23C74D")
PDF4Ax;
#endif
#endif /* __PDF4AxLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


