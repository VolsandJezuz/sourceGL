#ifndef GAMEDETECTION_H
#define GAMEDETECTION_H

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _WIN32_DCOM

#include <atomic>
#include <mutex>
#include <Wbemidl.h>
#include <Windows.h>

namespace sGL {

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

class gameDetection
{
public:
	static gameDetection& instance();
	bool wmiInitialize();
	void processDetected();

	std::atomic<bool> bWMI;
	std::atomic<bool> bProcessDetected;
	std::mutex processMutex;
	WCHAR processName[256];
	WCHAR processPath[MAX_PATH];
	WCHAR processPID[11];
	HANDLE hProcHandle;

private:
	gameDetection();
	~gameDetection();
	void wmiCleanup();

	bool bCIS;
	IWbemLocator *pLoc;
	IWbemServices *pSvc;
	IUnsecuredApartment* pUnsecApp;
	EventSink* pSink;
	IUnknown* pStubUnk;
	IWbemObjectSink* pStubSink;
};

VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

} // namespace sGL

#endif // GAMEDETECTION_H