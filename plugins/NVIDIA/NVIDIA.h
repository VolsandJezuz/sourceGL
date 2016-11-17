#ifndef PLUGINS_NVIDIA_NVIDIA_H
#define PLUGINS_NVIDIA_NVIDIA_H

#include "ui_NVIDIA.h"
#include "plugin.h"
#include <QtWidgets/qdialog.h>

#ifdef PLUGIN_NVIDIA_EXPORTS
#define PLUGIN_NVIDIA_API __declspec(dllexport)
#else
#define PLUGIN_NVIDIA_API __declspec(dllimport)
#endif // PLUGIN_NVIDIA_EXPORTS

#define PLUGIN_NVIDIA_MAJOR_VER 0
#define PLUGIN_NVIDIA_MINOR_VER 1
#define PLUGIN_NVIDIA_PATCH_VER 1
#define PLUGIN_NVIDIA_VER MAKE_SEMANTIC_VERSION(PLUGIN_NVIDIA_MAJOR_VER, PLUGIN_NVIDIA_MINOR_VER, PLUGIN_NVIDIA_PATCH_VER)

namespace nvidia {

class PLUGIN_NVIDIA_API PluginNVIDIA : public commondll::Plugin
{
public:
	PluginNVIDIA();
	~PluginNVIDIA() = default;
};

class NVIDIA : public QDialog
{
	Q_OBJECT

public:
	NVIDIA(QWidget *parent = 0);
	~NVIDIA() = default;

private:
	Ui::NVIDIAClass ui;
};

} // namespace nvidia

extern "C" PLUGIN_NVIDIA_API commondll::Plugin* createPlugin();
extern "C" PLUGIN_NVIDIA_API void destroyPlugin();

#endif // PLUGINS_NVIDIA_NVIDIA_H