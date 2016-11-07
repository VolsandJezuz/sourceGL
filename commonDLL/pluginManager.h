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

class pluginManager
{
public:
	static COMMONDLL_API pluginManager& instance();
	COMMONDLL_API void displayError(LPCWSTR pwszError);
	COMMONDLL_API void displayErrorA(LPCSTR pszError);
	COMMONDLL_API plugin* loadPlugin(const std::wstring& pluginName);
	COMMONDLL_API void unloadPlugin(plugin*& plugin);

private:
	typedef std::map<std::wstring, plugin*> pluginMap;
	typedef std::map<std::wstring, HMODULE> libraryMap;
	typedef void (*fnDisplayError_t)(LPCWSTR pwszError);
	typedef void (*fnDisplayErrorA_t)(LPCSTR pszError);

	pluginManager();
	~pluginManager();

	pluginMap m_plugins;
	libraryMap m_libs;
	HMODULE hDLL;
	fnDisplayError_t fnDisplayError;
	fnDisplayErrorA_t fnDisplayErrorA;
};

} // namespace commondll

#endif // COMMONDLL__PLUGINMANAGER_H_