#include "sourceGameLounge.h"
#include "gameDetection.h"
#include "pluginManager.h"
#include <vector>

QApplication* g_a;
sGL::sourceGameLounge* g_w;

int main(int argc, char *argv[])
{
	HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);

	g_a = new QApplication(argc, argv);
	g_w = new sGL::sourceGameLounge();

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

	std::vector<commondll::plugin*> plugins;
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
				plugins.push_back(plugin);
		} while (FindNextFile(fileHandle, &fileData));
	}

	if (!sGL::gameDetection::instance().wmiInitialize())
		return true;

	(*g_w).show();
	int ret = (*g_a).exec();

	try
	{
		sGL::config::instance().save(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	for (commondll::plugin* plugin : plugins)
		commondll::pluginManager::instance().unloadPlugin(plugin);

	delete g_w;
	g_w = NULL;
	delete g_a;
	g_a = NULL;

	return ret;
}