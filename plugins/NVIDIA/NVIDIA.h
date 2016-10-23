#ifndef NVIDIA_H
#define NVIDIA_H

#include <QtWidgets/QDialog>
#include "ui_NVIDIA.h"

struct NvStruct
{
	void Start();
	void Stop();

	inline ~NvStruct()
	{
		Stop();
	}
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

#endif