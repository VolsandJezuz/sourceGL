#define STRICT
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501
#define WIN32_LEAN_AND_MEAN

#include "sourceGameLounge.h"
#include "config.h"
#include "gameDetection.h"
#include "pluginManager.h"
#include <QtWidgets/QApplication>
#include <Windows.h>

int main(int argc, char *argv[])
{
	HRESULT hres;

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
		displayError(std::to_wstring(hres).c_str());

	QApplication a(argc, argv);
	sGL::sourceGameLounge w;

	try
	{
		sGL::config::instance().load(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	try
	{
		sGL::config::instance().save(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	WIN32_FIND_DATA fileData;
	HANDLE fileHandle = FindFirstFile(TEXT(".\\plugins\\*.dll"), &fileData);
	if (fileHandle == (void*)ERROR_INVALID_HANDLE || fileHandle == (void*)ERROR_FILE_NOT_FOUND)
		displayError(L"No plugins were found.");
	else
	{
		do
		{
			commondll::plugin* plugin = commondll::pluginManager::instance().loadPlugin(fileData.cFileName);
			if (plugin != NULL)
				w.plugins.push_back(plugin);
		} while (FindNextFile(fileHandle, &fileData));
	}

	if (!sGL::gameDetection::instance().wmiInitialize())
		return true;

	w.show();
	int ret = a.exec();

	try
	{
		sGL::config::instance().save(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	for (commondll::plugin* plugin : w.plugins)
		commondll::pluginManager::instance().unloadPlugin(plugin);

	return ret;
}