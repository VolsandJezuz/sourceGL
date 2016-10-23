#ifndef WMIEVENTSINK_H
#define WMIEVENTSINK_H

#define _WIN32_DCOM

#include <comdef.h>
#include <Wbemidl.h>
#include <thread>
#include <mutex>
#include <atomic>

#pragma comment(lib, "wbemuuid.lib")

extern std::atomic<bool> g_bWMI;
extern std::mutex processMutex;
extern std::atomic<bool> g_bProcessDetected;
extern std::thread g_thread1;

extern WCHAR g_processName[256];
extern WCHAR g_processPath[256];
extern WCHAR g_processPID[256];

bool wmiInitialize();
void wmiCleanup();
void processDetected();
VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

class EventSink : public IWbemObjectSink
{
	LONG m_lRef;
	bool bDone;

public:
	EventSink() {m_lRef = 0;}
	~EventSink() {bDone = true;}
	
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		void** ppv);

    virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray);
	
	virtual HRESULT STDMETHODCALLTYPE SetStatus(
		LONG lFlags,
		HRESULT hResult,
		BSTR strParam,
		IWbemClassObject __RPC_FAR *pObjParam);
};

#endif