// PDF4Ax.cpp : CPDF4AxApp および DLL 登録の実装

#include "stdafx.h"
#include "PDF4Ax.h"

#include <PDFSpecImports.h>

#pragma comment(lib, "PDFSpec.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CPDF4AxApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xD3444B56, 0xF4FE, 0x4BC4, { 0x90, 0x35, 0x71, 0x84, 0x2C, 0xF3, 0xE8, 0x69 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CPDF4AxApp::InitInstance - DLL 初期化

BOOL CPDF4AxApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit) {
		TCHAR tcMe[MAX_PATH] = {0};
		GetModuleFileName(AfxGetInstanceHandle(), tcMe, MAX_PATH);
		PathRemoveFileSpec(tcMe);
		TCHAR tcDir[MAX_PATH] = {0};
		PathCombine(tcDir, tcMe, _T("share\\poppler"));
		//Ext::CreateGlobalParams(CT2A(tcDir));
	}

	return bInit;
}



// CPDF4AxApp::ExitInstance - DLL 終了

int CPDF4AxApp::ExitInstance()
{
	//Ext::DestroyGlobalParams();

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - エントリをシステム レジストリに追加します。

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - エントリをレジストリから削除します。

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
