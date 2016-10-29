#include "NVIDIA.h"

pluginNVIDIA* g_pluginNVIDIA = NULL;

pluginNVIDIA::pluginNVIDIA(void)
{
	commonDLL::instance().addName("NVIDIA");
}

pluginNVIDIA::~pluginNVIDIA(void)
{
}

NVIDIA::NVIDIA(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
}

NVIDIA::~NVIDIA()
{
}

extern "C" PLUGINNVIDIA_API plugin* createPlugin(void)
{
	g_pluginNVIDIA = new pluginNVIDIA();
	return g_pluginNVIDIA;
}

extern "C" PLUGINNVIDIA_API void destroyPlugin(void)
{
	delete g_pluginNVIDIA;
	g_pluginNVIDIA = NULL;
}