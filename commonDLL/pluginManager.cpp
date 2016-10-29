#include "pluginManager.h"
#include <iostream>

typedef plugin* (*fnCreatePlugin)(void);
typedef void (*fnDestroyPlugin)(void);

plugin* pluginManager::loadPlugin(const std::wstring& pluginName)
{
	plugin* plugin = NULL;
	pluginMap::iterator iter = m_plugins.find(pluginName);
	if (iter == m_plugins.end())
	{
		HMODULE hModule = LoadLibraryW(pluginName.c_str());
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
				{
					std::wcerr << "ERROR: Could not load plugin from " << pluginName << std::endl;
					FreeLibrary(hModule);
				}
			}
			else
			{
				std::wcerr << "ERROR: Could not find symbol \"createPlugin\" in " << pluginName << std::endl;
				FreeLibrary(hModule);
			}
		}
		else
			std::wcerr << "ERROR: Could not load library: " << pluginName << std::endl;
	}
	else
	{
		std::wcout << "INFO: Library \"" << pluginName << "\" already loaded." << std::endl;
		plugin = iter->second;
	}

	return plugin;
}

void pluginManager::unloadPlugin(plugin*& plugin)
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
				std::wcerr << "ERROR: Unable to find symbol \"destroyPlugin\" in library \"" << plugin->getName() << std::endl;

			FreeLibrary(hModule);
			m_libs.erase(iter);
		}
		else
			std::cout << "WARNING: Trying to unload a plugin that is already unloaded or has never been loaded." << std::endl;

		plugin = NULL;
	}
}