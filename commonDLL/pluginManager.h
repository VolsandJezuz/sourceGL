#ifndef COMMONDLL__PLUGINMANAGER_H_
#define COMMONDLL__PLUGINMANAGER_H_

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "commonDLL.h"
#include "plugin.h"
#include <map>
#include <string>
#include <Windows.h>

namespace commondll {

class PluginManager
{
public:
	static COMMONDLL_API PluginManager& instance();
	COMMONDLL_API void displayError(LPCWSTR pwszError) const;
	COMMONDLL_API void displayErrorA(LPCSTR pszError) const;
	COMMONDLL_API Plugin* loadPlugin(const std::wstring& pluginName);
	COMMONDLL_API void unloadPlugin(Plugin*& plugin);

private:
	typedef std::map<std::wstring, Plugin*> pluginMap;
	typedef std::map<std::wstring, HMODULE> libraryMap;
	typedef void (*fnDisplayError_t)(LPCWSTR pwszError);
	typedef void (*fnDisplayErrorA_t)(LPCSTR pszError);

	PluginManager();
	~PluginManager();

	pluginMap m_plugins;
	libraryMap m_libs;
	HMODULE hDLL;
	fnDisplayError_t fnDisplayError;
	fnDisplayErrorA_t fnDisplayErrorA;
};

} // namespace commondll

#endif // COMMONDLL__PLUGINMANAGER_H_