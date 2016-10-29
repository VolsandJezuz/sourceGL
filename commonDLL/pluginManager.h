#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include "plugin.h"

class pluginManagerPimpl;

class COMMONDLL_API pluginManager
{
public:
	static pluginManager& instance();

	plugin* loadPlugin(const std::wstring& pluginName);
	void unloadPlugin(plugin*& plugin);

private:
	pluginManager(void);
	~pluginManager(void);
	
	pluginManagerPimpl* m_implementation;
};

#endif