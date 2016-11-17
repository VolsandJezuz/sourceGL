#include "sourceGameLounge.h"
#include "gameDetection.h"
#include "pluginManager.h"
#include <vector>

QApplication* g_pApplication;
sgl::SourceGameLounge* g_pWindow;

int main(int argc, char *argv[])
{
	HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);

	g_pApplication = new QApplication(argc, argv);
	g_pWindow = new sgl::SourceGameLounge();

	try
	{
		sgl::Config::instance().load(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	try
	{
		sgl::Config::instance().save(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	std::vector<commondll::Plugin*> plugins;
	WIN32_FIND_DATA fileData;

	HANDLE fileHandle = FindFirstFile(TEXT(".\\plugins\\*.dll"), &fileData);
	if (fileHandle == (void*)ERROR_INVALID_HANDLE || fileHandle == (void*)ERROR_FILE_NOT_FOUND)
		displayError(L"No plugins were found.");
	else
	{
		do
		{
			commondll::Plugin* plugin = commondll::PluginManager::instance().loadPlugin(fileData.cFileName);
			if (plugin != NULL)
				plugins.push_back(plugin);
		} while (FindNextFile(fileHandle, &fileData));
	}

	if (!sgl::GameDetection::instance().wmiInitialize(true))
		return true;

	(*g_pWindow).show();
	int ret = (*g_pApplication).exec();

	try
	{
		sgl::Config::instance().save(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}

	for (commondll::Plugin* plugin : plugins)
		commondll::PluginManager::instance().unloadPlugin(plugin);

	delete g_pWindow;
	g_pWindow = NULL;
	delete g_pApplication;
	g_pApplication = NULL;

	return ret;
}