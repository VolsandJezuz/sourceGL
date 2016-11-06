#include "pluginManager.h"

namespace commondll {

typedef plugin* (*fnCreatePlugin)();
typedef void(*fnDestroyPlugin)();

COMMONDLL_API pluginManager& pluginManager::instance()
{
	static pluginManager pluginManager;
	return pluginManager;
}

COMMONDLL_API void pluginManager::displayError(LPCWSTR pwszError)
{
	fnDisplayError(pwszError);
}

COMMONDLL_API void pluginManager::displayErrorA(LPCSTR pszError)
{
	fnDisplayErrorA(pszError);
}

COMMONDLL_API plugin* pluginManager::loadPlugin(const std::wstring& pluginName)
{
	std::wstring pwszError;
	plugin* plugin = NULL;
	pluginMap::iterator iter = m_plugins.find(pluginName);
	if (iter == m_plugins.end())
	{
		HMODULE hModule = LoadLibraryW((L".\\plugins\\" + pluginName).c_str());
		if (hModule != NULL)
		{
			fnCreatePlugin createPlugin = (fnCreatePlugin)GetProcAddress(hModule, "createPlugin");
			if (createPlugin != NULL)
			{
				plugin = createPlugin();
				if (plugin != NULL)
				{
					plugin->setName(pluginName);
					m_plugins.insert(pluginMap::value_type(pluginName, plugin));
					m_libs.insert(libraryMap::value_type(pluginName, hModule));
				}
				else
					FreeLibrary(hModule);
			}
			else
			{
				pwszError = L"Could not find symbol \"createPlugin\" in ";
				FreeLibrary(hModule);
			}
		}
		else
			pwszError = L"Could not load library: ";
	}
	else
	{
		pwszError = L"Already loaded library: ";
		plugin = iter->second;
	}

	if (!pwszError.empty())
		displayError((pwszError + pluginName).c_str());

	return plugin;
}

COMMONDLL_API void pluginManager::unloadPlugin(plugin*& plugin)
{
	if (plugin != NULL)
	{
		libraryMap::iterator iter = m_libs.find(plugin->getName());
		if (iter != m_libs.end())
		{
			m_plugins.erase(plugin->getName());

			HMODULE hModule = iter->second;
			fnDestroyPlugin destroyPlugin = (fnDestroyPlugin)GetProcAddress(hModule, "destroyPlugin");
			if (destroyPlugin != NULL)
				destroyPlugin();
			else
				displayError((L"Unable to find symbol \"destroyPlugin\" in library: " + plugin->getName()).c_str());

			FreeLibrary(hModule);
			m_libs.erase(iter);
		}
		else
			displayError(L"Trying to unload a plugin that is already unloaded or has never been loaded.");

		plugin = NULL;
	}
}

pluginManager::pluginManager()
{
	hDLL = LoadLibrary(TEXT("sourceGameLounge.exe"));
	if (hDLL != NULL)
	{
		fnDisplayError = (fnDisplayError_t)GetProcAddress(hDLL, "displayError");
		fnDisplayErrorA = (fnDisplayErrorA_t)GetProcAddress(hDLL, "displayErrorA");
	}
}

pluginManager::~pluginManager()
{
	if (hDLL != NULL)
		FreeLibrary(hDLL);
}

} // namespace commondll