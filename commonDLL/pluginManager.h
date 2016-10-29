#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include "commonDLL.h"
#include "plugin.h"
#include <Windows.h>
#include <string>
#include <map>

class COMMONDLL_API pluginManager
{
public:
	pluginManager(void);
	~pluginManager(void);

	plugin* loadPlugin(const std::wstring& pluginName);
	void unloadPlugin(plugin*& plugin);

private:
	typedef std::map<std::wstring, plugin*> pluginMap;
	typedef std::map<std::wstring, HMODULE> libraryMap;

	pluginMap m_plugins;
	libraryMap m_libs;
};

#endif