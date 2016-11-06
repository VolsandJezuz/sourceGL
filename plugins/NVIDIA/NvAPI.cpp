#include "NvAPI.h"
#include "pluginManager.h"

namespace nvidia {

bool NvAPI::Initialize()
{
	hDLL = LoadLibraryW(TEXT("nvapi.dll"));
	if (hDLL == NULL)
	{
		commondll::pluginManager::instance().displayError(L"Load NvAPI DLL failed.");

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

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_Initialize)();
	if (status != NVAPI_OK)
	{
		commondll::pluginManager::instance().displayError(L"NvAPI initialization failed.");

		return false;
	}

	return true;
}

int NvAPI::EnumNvidiaDisplayHandle(int thisEnum, int* pNvDispHandle)
{
	int DispCount = 0;

	int enumNvDispHandle;

	_NvAPI_Status status = NVAPI_OK;
	while (status != NVAPI_END_ENUMERATION)
	{
		status = (_NvAPI_Status)(*NvAPI_EnumNvidiaDisplayHandle)(DispCount, &enumNvDispHandle);
		if (status == NVAPI_OK)
		{
			if (DispCount == thisEnum)
				*pNvDispHandle = enumNvDispHandle;

			++DispCount;
		}
		else if (status != NVAPI_END_ENUMERATION)
			commondll::pluginManager::instance().displayError(L"NvAPI display handle enumeration failed.");
	}

	return DispCount;
}

char* NvAPI::GetAssociatedNvidiaDisplayName(int nDisp, char* szDispName)
{
	int NvDispHandle;
	EnumNvidiaDisplayHandle(nDisp, &NvDispHandle);

	(*NvAPI_GetAssociatedNvidiaDisplayName)(NvDispHandle, szDispName);

	return szDispName;
}

int NvAPI::EnumPhysicalGPUs(int** nvGPUHandle)
{
	int GpuCount;

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_EnumPhysicalGPUs)(nvGPUHandle, &GpuCount);
	if (status != NVAPI_OK)
		commondll::pluginManager::instance().displayError(L"NvAPI physical GPU enumeration failed.");

	return GpuCount;
}

NvAPI::NV_DISPLAY_DVC_INFO_EX NvAPI::GetDVCInfoEx(int nDisp)
{
	NV_DISPLAY_DVC_INFO_EX info;

	int NvDispHandle;
	EnumNvidiaDisplayHandle(nDisp, &NvDispHandle);

	info.version = sizeof(NV_DISPLAY_DVC_INFO_EX) | 0x10000;
	(*NvAPI_GetDVCInfoEx)(NvDispHandle, 0, &info);

	return info;
}

NvAPI::NV_GPU_COOLER_SETTINGS NvAPI::GPU_GetCoolerSettings(int nGPU)
{
	NV_GPU_COOLER_SETTINGS settings;

	int* nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
	EnumPhysicalGPUs(nvGPUHandle);

	settings.version = sizeof(NV_GPU_COOLER_SETTINGS) | 0x30000;
	(*NvAPI_GPU_GetCoolerSettings)(nvGPUHandle[nGPU], 7, &settings);

	return settings;
}

char* NvAPI::GPU_GetFullName(int nGPU, char* szName)
{
	int* nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
	EnumPhysicalGPUs(nvGPUHandle);

	(*NvAPI_GPU_GetFullName)(nvGPUHandle[nGPU], szName);

	return szName;
}

bool NvAPI::SetDVCInfoEx(int nDisp, int level)
{
	int NvDispHandle;
	EnumNvidiaDisplayHandle(nDisp, &NvDispHandle);

	NV_DISPLAY_DVC_INFO_EX oldInfo = GetDVCInfoEx(nDisp);

	NV_DISPLAY_DVC_INFO_EX info;
	info.version = oldInfo.version;
	info.currentLevel = level;
	info.minLevel = oldInfo.minLevel;
	info.maxLevel = oldInfo.maxLevel;
	info.defaultLevel = oldInfo.defaultLevel;

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_SetDVCLevelEx)(NvDispHandle, 0, &info);
	if (status != NVAPI_OK)
		return false;

	return true;
}

bool NvAPI::SetCoolerLevels(int nGPU, int nCooler, int newLevel)
{
	int* nvGPUHandle[NVAPI_MAX_PHYSICAL_GPUS];
	EnumPhysicalGPUs(nvGPUHandle);

	NV_GPU_COOLER_SETTINGS settings = GPU_GetCoolerSettings(nGPU);

	NV_GPU_COOLER_LEVELS levels;
	levels.version = sizeof(NV_GPU_COOLER_LEVELS) | 0x10000;

	for (unsigned int x = 0; x < settings.count; x++)
	{
		levels.Level[x].level = settings.Coolers[x].currentLevel;
		levels.Level[x].policy = settings.Coolers[x].currentPolicy;
	}

	levels.Level[nCooler].level = newLevel;
	levels.Level[nCooler].policy = 1;

	_NvAPI_Status status = (_NvAPI_Status)(*NvAPI_GPU_SetCoolerLevels)(nvGPUHandle[nGPU], 7, &levels);
	if (status != NVAPI_OK)
		return false;

	return true;
}

void NvAPI::Stop() const
{
	(*NvAPI_Unload)();
	FreeLibrary(hDLL);
}

} // namespace nvidia