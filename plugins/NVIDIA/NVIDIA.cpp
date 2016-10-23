#define _DO_NOT_EXPORT

#include "NVIDIA.h"
#include "../../libsgl/module.h"

static NvStruct *NvObject = nullptr;

SGL_DECLARE_MODULE()

bool sgl_module_load(void)
{
	NvObject = new NvStruct;
	NvObject->Start();
	return true;
}

void obs_module_unload(void)
{
	delete NvObject;
	NvObject = nullptr;
}

void NvStruct::Start()
{
	
}

void NvStruct::Stop()
{

}

NVIDIA::NVIDIA(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

NVIDIA::~NVIDIA()
{

}