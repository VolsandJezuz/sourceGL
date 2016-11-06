#include "NVIDIA.h"
#include "NvAPI.h"
#include "NVIDIAconfig.h"
#include "commonDLL.h"
#include "pluginManager.h"

namespace nvidia {

static pluginNVIDIA* g_pluginNVIDIA = NULL;
static NvAPI* g_cNvAPI = NULL;

pluginNVIDIA::pluginNVIDIA()
{
	commondll::commonDLL::instance().addName("NVIDIA");

	try
	{
		NVIDIAconfig::instance().load(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::pluginManager::instance().displayErrorA(e.what());
	}

	try
	{
		NVIDIAconfig::instance().save(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::pluginManager::instance().displayErrorA(e.what());
	}
}

pluginNVIDIA::~pluginNVIDIA()
{
	try
	{
		NVIDIAconfig::instance().save(".\\configs\\plugins\\NVIDIA_config.xml");
	}
	catch (std::exception &e)
	{
		commondll::pluginManager::instance().displayErrorA(e.what());
	}

	g_cNvAPI->Stop();

	delete g_cNvAPI;
	g_cNvAPI = NULL;
}

NVIDIA::NVIDIA(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

} // namespace nvidia

extern "C" PLUGIN_NVIDIA_API commondll::plugin* createPlugin()
{
	nvidia::g_cNvAPI = new nvidia::NvAPI();

	if (!nvidia::g_cNvAPI->Initialize())
	{
		if (nvidia::g_cNvAPI->hDLL)
			nvidia::g_cNvAPI->Stop();

		delete nvidia::g_cNvAPI;
		nvidia::g_cNvAPI = NULL;

		return NULL;
	}

	nvidia::g_pluginNVIDIA = new nvidia::pluginNVIDIA();

	return nvidia::g_pluginNVIDIA;
}

extern "C" PLUGIN_NVIDIA_API void destroyPlugin()
{
	delete nvidia::g_pluginNVIDIA;
	nvidia::g_pluginNVIDIA = NULL;
}