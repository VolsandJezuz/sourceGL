#include "sourceGameLounge.h"
#include "wmiEventSink.h"

static IWbemLocator *pLoc = NULL;
static IWbemServices *pSvc = NULL;
static IUnsecuredApartment* pUnsecApp = NULL;
static EventSink* pSink;
static IUnknown* pStubUnk = NULL;
static IWbemObjectSink* pStubSink = NULL;
std::atomic<bool> g_bWMI(false);
static HANDLE g_hProcHandle;

bool wmiInitialize()
{
	HRESULT hres;

	hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		displayError(TEXT("COM library initialization failed."));
		return false;
	}

	static bool bCIS;
	if (!bCIS)
	{
		hres = CoInitializeSecurity(
			NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL);

		if (FAILED(hres))
		{
			displayError(TEXT("COM security initialization failed."));
			CoUninitialize();
			return false;
		}
		bCIS = true;
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		displayError(TEXT("IWbemLocator object creation failed."));
		CoUninitialize();
		return false;
	}

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc);

	if (FAILED(hres))
	{
		displayError(TEXT("Connect to WMI namespace failed."));
		pLoc->Release();
		CoUninitialize();
		return false;
	}

    hres = CoSetProxyBlanket(
		pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);

	if (FAILED(hres))
	{
		displayError(TEXT("Setting WMI proxy blanket failed."));
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	hres = CoCreateInstance(
		CLSID_UnsecuredApartment,
		NULL, 
        CLSCTX_LOCAL_SERVER,
		IID_IUnsecuredApartment,
		(void**)&pUnsecApp);

	pSink = new EventSink;
	pSink->AddRef();
	pUnsecApp->CreateObjectStub(pSink, &pStubUnk);
	pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **) &pStubSink);

	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"),
		_bstr_t("SELECT * "
			"FROM __InstanceCreationEvent WITHIN 1 "
			"WHERE TargetInstance ISA 'Win32_Process'"),
		WBEM_FLAG_SEND_STATUS,
		NULL,
		pStubSink);

	if (FAILED(hres))
	{
		displayError(TEXT("Call to ExecNotificationQueryAsync failed."));
		pSvc->Release();
		pLoc->Release();
		pUnsecApp->Release();
		pStubUnk->Release();
		pSink->Release();
		pStubSink->Release();
		CoUninitialize();
		return false;
	}

	return g_bWMI = true;
}

void wmiCleanup()
{
	pSvc->CancelAsyncCall(pStubSink);
    pSvc->Release();
    pLoc->Release();
    pUnsecApp->Release();
    pStubUnk->Release();
    pSink->Release();
    pStubSink->Release();
    CoUninitialize();
	g_bWMI = false;
}

void processDetected()
{
	g_hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)_wtoi(g_processPID));
	HANDLE hNewHandle;
	if (!RegisterWaitForSingleObject(&hNewHandle, g_hProcHandle, WaitOrTimerCallback, NULL, INFINITE, WT_EXECUTEONLYONCE))
		CloseHandle(g_hProcHandle);

	wmiCleanup();
}

VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter,
								  BOOLEAN TimerOrWaitFired)
{
	displayError(g_processName);

	processMutex.lock();
	g_bProcessDetected = false;
	processMutex.unlock();

	wmiInitialize();
	CloseHandle(g_hProcHandle);
}