#ifndef NVIDIA_H
#define NVIDIA_H

#include "plugin.h"
#include <QtWidgets/QDialog>
#include "ui_NVIDIA.h"

#ifdef PLUGINNVIDIA_EXPORTS
#define PLUGINNVIDIA_API __declspec(dllexport)
#else
#define PLUGINNVIDIA_API __declspec(dllimport)
#endif

class PLUGINNVIDIA_API pluginNVIDIA : public plugin
{
public:
	pluginNVIDIA(void);
	~pluginNVIDIA(void);
};

class NVIDIA : public QDialog
{
	Q_OBJECT

public:
	NVIDIA(QWidget *parent = 0);
	~NVIDIA();

private:
	Ui::NVIDIAClass ui;
};

extern "C" PLUGINNVIDIA_API plugin* createPlugin(void);
extern "C" PLUGINNVIDIA_API void destroyPlugin(void);

#endif