#include "gameDetection.h"
#include "sourceGameLounge.h"
#include <chrono>
#include <thread>
#include <comdef.h>

namespace sgl {

VOID CALLBACK waitOrTimerCallback(PVOID lpParameter, BOOLEAN timerOrWaitFired);

EventSink::EventSink()
{
	m_lRef = 0;
}

ULONG EventSink::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
	LONG lRef = InterlockedDecrement(&m_lRef);
	if (lRef == 0)
		delete this;

	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
	{
		*ppv = (IWbemObjectSink *) this;
		AddRef();

		return WBEM_S_NO_ERROR;
	}
	else
		return E_NOINTERFACE;
}

HRESULT EventSink::Indicate(long lObjectCount, IWbemClassObject **apObjArray)
{
	if (!GameDetection::instance().destructing)
	{
		EnterCriticalSection(&GameDetection::instance().criticalSection);

		HRESULT hr = S_OK;
		_variant_t vtProp;

		for (int i = 0; i < lObjectCount; i++)
		{
			if (GameDetection::instance().bProcessDetected || GameDetection::instance().destructing)
			{
				LeaveCriticalSection(&GameDetection::instance().criticalSection);

				return WBEM_S_NO_ERROR;
			}

			hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
			if (!FAILED(hr))
			{
				IUnknown* str = vtProp;
				hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&apObjArray[i]));
				if (SUCCEEDED(hr))
				{
					_variant_t cn;

					hr = apObjArray[i]->Get(L"Caption", 0, &cn, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
							lstrcpyW(GameDetection::instance().processName, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));
						else
							lstrcpyW(GameDetection::instance().processName, cn.bstrVal);

						WCHAR *exes[] = {L"csgo.exe", L"hl2.exe", NULL}; // temporary
						int n = 1; // temporary

						while ((exes[n - 1] != NULL) && wcscmp(GameDetection::instance().processName, exes[n - 1])) // temporary
							++n; // temporary

						if (n < 3) // temporary
							GameDetection::instance().bProcessDetected = true;
					}

					VariantClear(&cn);

					/* Use process path to make sure it's the desired game

					hr = apObjArray[i]->Get(L"CommandLine", 0, &cn, NULL, NULL);
					if (SUCCEEDED(hr))
					{
					if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
					lstrcpyW(gameDetection::instance().processPath, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));
					else
					lstrcpyW(gameDetection::instance().processPath, cn.bstrVal);
					}

					VariantClear(&cn);*/

					if (GameDetection::instance().bProcessDetected)
					{
						hr = apObjArray[i]->Get(L"Handle", 0, &cn, NULL, NULL);
						if (SUCCEEDED(hr))
						{
							if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
							{
								lstrcpyW(GameDetection::instance().processPID, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));

								GameDetection::instance().bProcessDetected = false;
							}
							else
							{
								lstrcpyW(GameDetection::instance().processPID, cn.bstrVal);

								GameDetection::instance().processDetected();
							}
						}
						else
							GameDetection::instance().bProcessDetected = false;

						VariantClear(&cn);
					}
				}
			}

			VariantClear(&vtProp);
		}

		LeaveCriticalSection(&GameDetection::instance().criticalSection);
	}

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR *pObjParam)
{
	return WBEM_S_NO_ERROR;
}

GameDetection& GameDetection::instance()
{
	static GameDetection gameDetection;
	return gameDetection;
}

bool GameDetection::wmiInitialize(bool initialize)
{
	HRESULT hres;

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		displayError(L"COM library initialization failed.");

		return false;
	}

	if (!bCIS)
	{
		hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (FAILED(hres))
		{
			displayError(L"COM security initialization failed.");
			CoUninitialize();

			return false;
		}

		bCIS = true;
	}

	pLoc = NULL;

	hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
	if (FAILED(hres))
	{
		displayError(L"IWbemLocator object creation failed.");
		CoUninitialize();

		return false;
	}

	pSvc = NULL;

	hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
	if (FAILED(hres))
	{
		displayError(L"Connect to WMI namespace failed.");
		pLoc->Release();
		CoUninitialize();

		return false;
	}

	hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hres))
	{
		displayError(L"Setting WMI proxy blanket failed.");
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();

		return false;
	}

	pUnsecApp = NULL;
	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL, CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment, (void**)&pUnsecApp);

	pSink = new EventSink;
	pSink->AddRef();

	pStubUnk = NULL;
	pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

	pStubSink = NULL;
	pStubUnk->QueryInterface(IID_IWbemObjectSink, (void **)&pStubSink);

	hres = pSvc->ExecNotificationQueryAsync(_bstr_t("WQL"), _bstr_t("SELECT * " "FROM __InstanceCreationEvent WITHIN 1 " "WHERE TargetInstance ISA 'Win32_Process'"), WBEM_FLAG_SEND_STATUS, NULL, pStubSink);
	if (FAILED(hres))
	{
		displayError(L"Call to ExecNotificationQueryAsync failed.");
		pSvc->Release();
		pLoc->Release();
		pUnsecApp->Release();
		pStubUnk->Release();
		pSink->Release();
		pStubSink->Release();
		CoUninitialize();

		return false;
	}

	if (initialize)
		InitializeCriticalSection(&instance().criticalSection);

	return bWMI = true;
}

void GameDetection::processDetected()
{
	hProcHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)_wtoi(processPID));
	HANDLE hNewHandle;

	if (!RegisterWaitForSingleObject(&hNewHandle, hProcHandle, waitOrTimerCallback, NULL, INFINITE, WT_EXECUTEONLYONCE))
	{
		displayError(L"Call to RegisterWaitForSingleObject failed.");
		CloseHandle(hProcHandle);
	}

	wmiCleanup();
}

GameDetection::GameDetection()
{
	bWMI = false;
	bProcessDetected = false;
	bCIS = false;
}

GameDetection::~GameDetection()
{
	destructing = true;

	if (bWMI)
		wmiCleanup();

	while (!TryEnterCriticalSection(&GameDetection::instance().criticalSection))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	LeaveCriticalSection(&GameDetection::instance().criticalSection);
	DeleteCriticalSection(&GameDetection::instance().criticalSection);
}

void GameDetection::wmiCleanup()
{
	pSvc->CancelAsyncCall(pStubSink);
	pSvc->Release();
	pLoc->Release();
	pUnsecApp->Release();
	pStubUnk->Release();
	pSink->Release();
	pStubSink->Release();
	CoUninitialize();

	bWMI = false;
}

VOID CALLBACK waitOrTimerCallback(PVOID lpParameter, BOOLEAN timerOrWaitFired)
{
	displayError(GameDetection::instance().processName); // remove after testing

	while (!GameDetection::instance().destructing)
	{
		if (!TryEnterCriticalSection(&GameDetection::instance().criticalSection))
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		else
			break;
	}

	if (!GameDetection::instance().destructing)
	{
		GameDetection::instance().bProcessDetected = false;

		LeaveCriticalSection(&GameDetection::instance().criticalSection);

		GameDetection::instance().wmiInitialize();
	}

	CloseHandle(GameDetection::instance().hProcHandle);
}

} // namespace sgl