#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "setup.h"
#include "config.h"
#include "sourceGameLounge.h"
#include <algorithm>
#include <Windows.h>

namespace sgl {

Setup& Setup::instance()
{
	static Setup setup;
	return setup;
}

void Setup::initialSetup()
{
	if (findSteamDir())
	{
		std::vector<std::wstring> steamIDs = Setup::enumSteamIDs();

		bool ret = Config::instance().steam32ID != 0;
		if (ret)
			ret = std::find(steamIDs.begin(), steamIDs.end(), std::to_wstring(Config::instance().steam32ID)) != steamIDs.end();
		
		if (!ret)
		{
			for (std::wstring steamID : steamIDs)
				displayError(steamID.c_str());
		}
	}
	else
		Config::instance().steam32ID = 0;
}

bool Setup::findSteamDir()
{
	HKEY hKey = NULL;
	DWORD keyType = REG_SZ;
	WCHAR keyData[MAX_PATH] = {0};
	DWORD keyDataSize = sizeof(keyData);

	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
	if (ret == ERROR_SUCCESS)
	{
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, (LPBYTE)keyData, &keyDataSize);
		if (ret == ERROR_SUCCESS)
			steamDir = keyData;

		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND)
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, (LPBYTE)keyData, &keyDataSize);
			if (ret == ERROR_SUCCESS)
				steamDir = keyData;

			RegCloseKey(hKey);
		}

		if (ret != ERROR_SUCCESS)
			return false;
	}

	return true;
}

std::vector<std::wstring> Setup::enumSteamIDs()
{
	std::vector<std::wstring> steamIDs;
	WIN32_FIND_DATA fileData;

	HANDLE fileHandle = FindFirstFileW((steamDir + L"\\userdata\\*").c_str(), &fileData);
	if (fileHandle != (void*)ERROR_INVALID_HANDLE && fileHandle != (void*)ERROR_FILE_NOT_FOUND)
	{
		do
		{
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::wstring(fileData.cFileName) != std::wstring(L".") && std::wstring(fileData.cFileName) != std::wstring(L".."))
				steamIDs.push_back(std::wstring(fileData.cFileName));
		} while (FindNextFileW(fileHandle, &fileData));
	}

	return steamIDs;
}

} // namesapce sgl