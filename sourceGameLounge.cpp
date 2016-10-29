#include "commonDLL.h"
#include "sourceGameLounge.h"
#include "wmiEventSink.h"

sourceGameLounge::sourceGameLounge(QWidget *parent) : QMainWindow(parent)
{
	commonDLL::instance().addName("sourceGameLounge");

	ui.setupUi(this);
}

sourceGameLounge::~sourceGameLounge()
{
	if (g_thread1.joinable())
		g_thread1.join();

	if (g_bWMI)
		wmiCleanup();
}
