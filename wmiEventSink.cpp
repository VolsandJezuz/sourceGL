#include "wmiEventSink.h"

std::mutex processMutex;
std::atomic<bool> g_bProcessDetected(false);
std::thread g_thread1;

WCHAR g_processName[256];
WCHAR g_processPath[256];
WCHAR g_processPID[256];

ULONG EventSink::AddRef()
{
	return InterlockedIncrement(&m_lRef);
}

ULONG EventSink::Release()
{
	LONG lRef = InterlockedDecrement(&m_lRef);
	if(lRef == 0)
		delete this;
	return lRef;
}

HRESULT EventSink::QueryInterface(REFIID riid,
								  void** ppv)
{
	if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
	{
		*ppv = (IWbemObjectSink *) this;
		AddRef();
		return WBEM_S_NO_ERROR;
	}
	else return E_NOINTERFACE;
}

HRESULT EventSink::Indicate(long lObjectCount,
							IWbemClassObject **apObjArray)
{
	HRESULT hr = S_OK;
	_variant_t vtProp;

	processMutex.lock();
	for (int i = 0; i < lObjectCount; i++)
	{
		if (g_bProcessDetected)
		{
			processMutex.unlock();
			return WBEM_S_NO_ERROR;
		}

		hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
		if (!FAILED(hr))
		{
			IUnknown* str = vtProp;
			hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast< void** >(&apObjArray[i]));
			if (SUCCEEDED(hr))
			{
				_variant_t cn;

				hr = apObjArray[i]->Get(L"Caption", 0, &cn, NULL, NULL);
				if (SUCCEEDED(hr))
				{
					if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
						lstrcpyW(g_processName, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));
					else
						lstrcpyW(g_processName, cn.bstrVal);

					WCHAR *exes[] = {L"csgo.exe", L"hl2.exe", NULL};
					int n = 1;
					while ((exes[n - 1] != NULL) && wcscmp(g_processName, exes[n - 1]))
						++n;
					if (n < 3)
						g_bProcessDetected = true;
				}

				VariantClear(&cn);

				/*hr = apObjArray[i]->Get(L"CommandLine", 0, &cn, NULL, NULL);
				if (SUCCEEDED(hr))
				{
					if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
						lstrcpyW(g_processPath, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));
					else
						lstrcpyW(g_processPath, cn.bstrVal);
				}
				VariantClear(&cn);*/

				if (g_bProcessDetected)
				{
					hr = apObjArray[i]->Get(L"Handle", 0, &cn, NULL, NULL);
					if (SUCCEEDED(hr))
					{
						if ((cn.vt == VT_NULL) || (cn.vt == VT_EMPTY) || (cn.vt == VT_ARRAY))
						{
							lstrcpyW(g_processPID, ((cn.vt == VT_NULL) ? L"NULL" : L"EMPTY"));

							g_bProcessDetected = false;
						}
						else
						{
							lstrcpyW(g_processPID, cn.bstrVal);

							if (g_thread1.joinable())
								g_thread1.join();
							g_thread1 = std::thread(processDetected);
						}
					}
					else
						g_bProcessDetected = false;

					VariantClear(&cn);
				}
			}
		}

		VariantClear(&vtProp);
	}
	processMutex.unlock();

	return WBEM_S_NO_ERROR;
}

HRESULT EventSink::SetStatus(LONG lFlags,
							 HRESULT hResult,
							 BSTR strParam,
							 IWbemClassObject __RPC_FAR *pObjParam)
{
	return WBEM_S_NO_ERROR;
}