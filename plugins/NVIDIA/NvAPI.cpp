#include "NvAPI.h"

bool sNvAPI::Start(LPTSTR pwszError)
{
	hDLL = LoadLibrary(TEXT("nvapi.dll"));
	if (hDLL == NULL)
	{
		lstrcpy(pwszError, TEXT("Load NvAPI DLL failed."));
		return false;
	}

	NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(hDLL, "nvapi_QueryInterface");
	NvAPI_Initialize = (NvAPI_Initialize_t)(*NvAPI_QueryInterface)(0x0150E828);
	NvAPI_EnumNvidiaDisplayHandle = (NvAPI_EnumNvidiaDisplayHandle_t)(*NvAPI_QueryInterface)(0x9ABDD40D);
	NvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUs_t)(*NvAPI_QueryInterface)(0xE5AC921F);
	NvAPI_GPU_GetFullName = (NvAPI_GPU_GetFullName_t)(*NvAPI_QueryInterface)(0xCEEE8E9F);
	NvAPI_GetAssociatedNvidiaDisplayName = (NvAPI_GetAssociatedNvidiaDisplayName_t)(*NvAPI_QueryInterface)(0x22A78B05);
	NvAPI_GetDVCInfoEx = (NvAPI_GetDVCInfoEx_t)(*NvAPI_QueryInterface)(0x0E45002D);
	NvAPI_SetDVCLevelEx = (NvAPI_SetDVCLevelEx_t)(*NvAPI_QueryInterface)(0x4A82C2B1);
	NvAPI_GPU_GetCoolerSettings = (NvAPI_GPU_GetCoolerSettings_t)(*NvAPI_QueryInterface)(0xDA141340);
	NvAPI_GPU_SetCoolerLevels = (NvAPI_GPU_SetCoolerLevels_t)(*NvAPI_QueryInterface)(0x891FA0AE);
	NvAPI_Unload = (NvAPI_Unload_t)(*NvAPI_QueryInterface)(0xD22BDD7E);

	_NvAPI_Status status;
	status = (_NvAPI_Status)(*NvAPI_Initialize)();
	if (status != NVAPI_OK)
	{
		lstrcpy(pwszError, TEXT("NvAPI initialization failed."));
		return false;
	}

	DispCount = 0;
	while (status != NVAPI_END_ENUMERATION)
	{
		int NvDispHandle;
		status = (_NvAPI_Status)(*NvAPI_EnumNvidiaDisplayHandle)(DispCount, &NvDispHandle);
		if (status == NVAPI_OK)
		{
			arr_NvDispHandle.push_back(NvDispHandle);
			++DispCount;
		}
		else if (status != NVAPI_END_ENUMERATION)
		{
			lstrcpy(pwszError, TEXT("NvAPI display handle enumeration failed."));
			return false;
		}
	}

	int n = 0;
	while (n < DispCount)
	{
		char szDisplayName[64];
		(*NvAPI_GetAssociatedNvidiaDisplayName)(arr_NvDispHandle[n], szDisplayName);
		if (szDisplayName != NULL)
		{
			std::string myString(szDisplayName);
			arr_szDisplayName.push_back(myString);
		}
		++n;
	}

	GpuCount = 0;
	status = (_NvAPI_Status)(*NvAPI_EnumPhysicalGPUs)(nvGPUHandle, &GpuCount);
	if (status != NVAPI_OK)
	{
		lstrcpy(pwszError, TEXT("NvAPI physical GPU enumeration failed."));
		return false;
	}

	n = 0;
	while (n < DispCount)
	{
		NV_DISPLAY_DVC_INFO_EX info;
		info.version = sizeof(NV_DISPLAY_DVC_INFO_EX) | 0x10000;
		(*NvAPI_GetDVCInfoEx)(arr_NvDispHandle[n], 0, &info);
		arr_DVCInfo.push_back(info);
		++n;
	}

	n = 0;
	while (n < GpuCount)
	{
		NV_GPU_COOLER_SETTINGS settings;
		settings.version = sizeof(NV_GPU_COOLER_SETTINGS) | 0x30000;
		(*NvAPI_GPU_GetCoolerSettings)(nvGPUHandle[n], 7, &settings);
		arr_CoolerInfo.push_back(settings);
		++n;
	}

	return true;
}

std::string sNvAPI::GPU_GetFullName(int nGPU) const
{
	std::string myString = "";
	char szName[64];
	(*NvAPI_GPU_GetFullName)(nvGPUHandle[nGPU], szName);
	if (szName != NULL)
	{
		std::string myString(szName);
		return myString;
	}
	return myString;
}

bool sNvAPI::SetDVCInfoEx(int nDisp, int level) const
{
	NV_DISPLAY_DVC_INFO_EX info;
	info.version = arr_DVCInfo[nDisp].version;
	info.currentLevel = level;
	info.minLevel = arr_DVCInfo[nDisp].minLevel;
	info.maxLevel = arr_DVCInfo[nDisp].maxLevel;
	info.defaultLevel = arr_DVCInfo[nDisp].defaultLevel;

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_SetDVCLevelEx)(arr_NvDispHandle[nDisp], 0, &info);
	if (status != NVAPI_OK)
		return false;

	return true;
}

bool sNvAPI::SetCoolerLevels(int nGPU, int nCooler, int newLevel) const
{
	NV_GPU_COOLER_LEVELS levels;
	levels.version = sizeof(NV_GPU_COOLER_LEVELS) | 0x10000;

	for (unsigned int x = 0; x < arr_CoolerInfo[nGPU].count; x++)
	{
		levels.Level[x].level = arr_CoolerInfo[nGPU].Coolers[x].currentLevel;
		levels.Level[x].policy = arr_CoolerInfo[nGPU].Coolers[x].currentPolicy;
	}

	levels.Level[nCooler].level = newLevel;
	levels.Level[nCooler].policy = 1;

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_GPU_SetCoolerLevels)(nvGPUHandle[nGPU], 7, &levels);
	if (status != NVAPI_OK)
		return false;

	return true;
}

void sNvAPI::Stop() const
{
	(*NvAPI_Unload)();
	FreeLibrary(hDLL);
}

/*DISPLAY_DEVICE DispDev;
ZeroMemory(&DispDev, sizeof(DispDev));
DispDev.cb = sizeof(DispDev);
if (EnumDisplayDevices(NULL, 0, &DispDev, 0))
displayError(DispDev.DeviceName);*/