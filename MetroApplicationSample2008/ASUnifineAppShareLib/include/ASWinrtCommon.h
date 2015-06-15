#pragma once

#include <shobjidl.h>
#include <ShlGuid.h>
#include "hstring.h"
#include "windows.applicationmodel.h"
#include "windows.management.deployment.h"
#include "appmodel.h"

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Management::Deployment;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Windows::System;
using namespace ABI::Windows::Storage;
using namespace ABI::Windows::Foundation::Collections;


//using namespace std;
//#pragma comment(lib,"Shlwapi.lib")

const CLSID CLSID_ApplicationActivationManager =
{0x45BA127D,0x10A8,0x46EA,{0x8A,0xB7,0x56,0xEA,0x90,0x78,0x94,0x3C}};

#if (NTDDI_VERSION >= NTDDI_WIN8)
typedef /* [v1_enum] */ 
enum ACTIVATEOPTIONS
{
	AO_NONE	= 0,
	AO_DESIGNMODE	= 0x1,
	AO_NOERRORUI	= 0x2,
	AO_NOSPLASHSCREEN	= 0x4
} 	ACTIVATEOPTIONS;

DEFINE_ENUM_FLAG_OPERATORS(ACTIVATEOPTIONS)
const IID IID_IApplicationActivationManager =
{0x2e941141,0x7f97,0x4756,{0xba,0x1d,0x9d,0xec,0xde,0x89,0x4a,0x3d}};

MIDL_INTERFACE("2e941141-7f97-4756-ba1d-9decde894a3d")
IApplicationActivationManager : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE ActivateApplication( 
		/* [in] */ __RPC__in LPCWSTR appUserModelId,
		/* [unique][in] */ __RPC__in_opt LPCWSTR arguments,
		/* [in] */ ACTIVATEOPTIONS options,
		/* [out] */ __RPC__out DWORD *processId) = 0;

	virtual HRESULT STDMETHODCALLTYPE ActivateForFile( 
		/* [in] */ __RPC__in LPCWSTR appUserModelId,
		/* [in] */ __RPC__in_opt IShellItemArray *itemArray,
		/* [unique][in] */ __RPC__in_opt LPCWSTR verb,
		/* [out] */ __RPC__out DWORD *processId) = 0;

	virtual HRESULT STDMETHODCALLTYPE ActivateForProtocol( 
		/* [in] */ __RPC__in LPCWSTR appUserModelId,
		/* [in] */ __RPC__in_opt IShellItemArray *itemArray,
		/* [out] */ __RPC__out DWORD *processId) = 0;

};

#endif // NTDDI_WIN8
