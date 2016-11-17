#ifndef GAMEDETECTION_H
#define GAMEDETECTION_H

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#include <atomic>
#include <Wbemidl.h>
#include <Windows.h>

namespace sgl {

class EventSink : public IWbemObjectSink
{
public:
	EventSink();
	~EventSink() = default;
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
	virtual HRESULT STDMETHODCALLTYPE Indicate(LONG lObjectCount, IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray);
	virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG lFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR *pObjParam);

private:
	LONG m_lRef;
};

class GameDetection
{
public:
	static GameDetection& instance();
	bool wmiInitialize(bool initialize = false);
	void processDetected();

	bool destructing;
	std::atomic<bool> bWMI;
	std::atomic<bool> bProcessDetected;
	CRITICAL_SECTION criticalSection;
	WCHAR processName[256];
	WCHAR processPath[MAX_PATH];
	WCHAR processPID[11];
	HANDLE hProcHandle;

private:
	GameDetection();
	~GameDetection();
	void wmiCleanup();

	bool bCIS;
	IWbemLocator* pLoc;
	IWbemServices* pSvc;
	IUnsecuredApartment* pUnsecApp;
	EventSink* pSink;
	IUnknown* pStubUnk;
	IWbemObjectSink* pStubSink;
};

} // namespace sgl

#endif // GAMEDETECTION_H