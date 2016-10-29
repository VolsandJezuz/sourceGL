#include "pluginManager.h"
#include "sourceGameLounge.h"
#include "config.h"
#include "wmiEventSink.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	sConfig sC;

	try
	{
		sC.load("config.xml");
	}
	catch (std::exception &e)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, NULL, 0);
		WCHAR *pwszError = new WCHAR[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, pwszError, size_needed);

		displayError(pwszError);

		delete[] pwszError;
	}

	std::vector<plugin*> plugins;

	WIN32_FIND_DATA fileData;
	HANDLE fileHandle = FindFirstFile(TEXT(".\\plugins\\*.dll"), &fileData);
	if (fileHandle == (void*)ERROR_INVALID_HANDLE || fileHandle == (void*)ERROR_FILE_NOT_FOUND)
		displayError(L"No plugins were found.");
	else
	{
		do
		{
			plugin* plugin = pluginManager::instance().loadPlugin(fileData.cFileName);
			if (plugin != NULL)
				plugins.push_back(plugin);
		} while (FindNextFile(fileHandle, &fileData));
	}

	if (!wmiInitialize())
	{
		return true;
	}

	QApplication a(argc, argv);
	sourceGameLounge w;
	w.show();
	int ret = a.exec();

	for (plugin* plugin : plugins)
		pluginManager::instance().unloadPlugin(plugin);

	try
	{
		sC.save("config.xml");
	}
	catch (std::exception &e)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, NULL, 0);
		WCHAR *pwszError = new WCHAR[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, pwszError, size_needed);

		displayError(pwszError);

		delete[] pwszError;
	}

	return ret;
}