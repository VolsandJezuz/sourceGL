#include "NVIDIA.h"
#include "NvAPI.h"
#include "NVIDIAconfig.h"
#include "commonDLL.h"
#include "pluginManager.h"

namespace nvidia {

static PluginNVIDIA* g_pPluginNVIDIA = NULL;
static NvAPI* g_pNvAPI = NULL;

PluginNVIDIA::PluginNVIDIA()
{
	commondll::CommonDLL::instance().addName("NVIDIA");
}

NVIDIA::NVIDIA(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

} // namespace nvidia

extern "C" PLUGIN_NVIDIA_API commondll::Plugin* createPlugin()
{
	nvidia::g_pNvAPI = new nvidia::NvAPI();

	if (!nvidia::g_pNvAPI->initialize())
	{
		delete nvidia::g_pNvAPI;
		nvidia::g_pNvAPI = NULL;

		return NULL;
	}

	nvidia::g_pPluginNVIDIA = new nvidia::PluginNVIDIA();

	try
	{
		nvidia::NVIDIAconfig::instance().load(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::PluginManager::instance().displayErrorA(e.what());
	}

	try
	{
		nvidia::NVIDIAconfig::instance().save(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::PluginManager::instance().displayErrorA(e.what());
	}

	return nvidia::g_pPluginNVIDIA;
}

extern "C" PLUGIN_NVIDIA_API void destroyPlugin()
{
	try
	{
		nvidia::NVIDIAconfig::instance().save(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::PluginManager::instance().displayErrorA(e.what());
	}

	delete nvidia::g_pNvAPI;
	nvidia::g_pNvAPI = NULL;

	delete nvidia::g_pPluginNVIDIA;
	nvidia::g_pPluginNVIDIA = NULL;
}