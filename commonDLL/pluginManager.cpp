#include "pluginManager.h"
#include <map>

typedef plugin* (*fnCreatePlugin)(void);
typedef void (*fnDestroyPlugin)(void);

typedef std::map<std::wstring, plugin*> pluginMap;
typedef std::map<std::wstring, HMODULE> libraryMap;

class pluginManagerPimpl
{
public:
	pluginMap m_plugins;
	libraryMap m_libs;
};

pluginManager::pluginManager(void)
{
	m_implementation = new pluginManagerPimpl();
}

pluginManager::~pluginManager(void)
{
	delete m_implementation;
}

pluginManager& pluginManager::instance()
{
	static pluginManager pluginManager;
	return pluginManager;
}

plugin* pluginManager::loadPlugin(const std::wstring& pluginName)
{
	std::wstring pwszError;
	plugin* plugin = NULL;
	pluginMap::iterator iter = m_implementation->m_plugins.find(pluginName);
	if (iter == m_implementation->m_plugins.end())
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
					m_implementation->m_plugins.insert(pluginMap::value_type(pluginName, plugin));
					m_implementation->m_libs.insert(libraryMap::value_type(pluginName, hModule));
				}
				else
				{
					pwszError = L"Could not load plugin from ";
					FreeLibrary(hModule);
				}
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
		pwszError = L"Alread loaded library: ";
		plugin = iter->second;
	}

	if (!pwszError.empty())
		displayError((pwszError + pluginName).c_str());

	return plugin;
}

void pluginManager::unloadPlugin(plugin*& plugin)
{
	if (plugin != NULL)
	{
		libraryMap::iterator iter = m_implementation->m_libs.find(plugin->getName());
		if (iter != m_implementation->m_libs.end())
		{
			m_implementation->m_plugins.erase(plugin->getName());

			HMODULE hModule = iter->second;
			fnDestroyPlugin destroyPlugin = (fnDestroyPlugin)GetProcAddress(hModule, "destroyPlugin");
			if (destroyPlugin != NULL)
				destroyPlugin();
			else
				displayError((L"Unable to find symbol \"destroyPlugin\" in library: " + plugin->getName()).c_str());
			
			FreeLibrary(hModule);
			m_implementation->m_libs.erase(iter);
		}
		else
			displayError(L"Trying to unload a plugin that is already unloaded or has never been loaded.");

		plugin = NULL;
	}
}