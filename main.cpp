#include "config.h"
#include "gameDetection.h"
#include "setup.h"
#include "sourceGameLounge.h"
#include "pluginManager.h"
#include <vector>

#define LOAD false
#define SAVE true

sgl::SourceGameLounge* g_pWindow = NULL;

void config(bool save)
{
	try
	{
		if (save)
			sgl::Config::instance().save(".\\configs\\config.xml");
		else
			sgl::Config::instance().load(".\\configs\\config.xml");
	}
	catch (std::exception &e)
	{
		displayErrorA(e.what());
	}
}

int main(int argc, char *argv[])
{
	HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);

	QApplication application(argc, argv);

	config(LOAD);
	config(SAVE);

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

	int ret = 1;

	if (sgl::GameDetection::instance().wmiInitialize(true))
	{
		g_pWindow = new sgl::SourceGameLounge(&application);
		g_pWindow->show();

		sgl::SetupPre* setupPre = new sgl::SetupPre();
		if (!setupPre->initialSetup())
			setupPre->pSetup->exec();

		delete setupPre;
		setupPre = NULL;

		ret = application.exec();

		delete g_pWindow;
		g_pWindow = NULL;
	}

	config(SAVE);

	for (commondll::Plugin* plugin : plugins)
		commondll::PluginManager::instance().unloadPlugin(plugin);

	return ret;
}