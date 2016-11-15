#include "sourceGameLounge.h"
#include "commonDLL.h"
#include <QtWidgets/QWhatsThis>
#include <string>
#include <chrono>

namespace sGL {

sourceGameLounge::sourceGameLounge(QWidget *parent) : QMainWindow(parent)
{
	commondll::commonDLL::instance().addName("sourceGameLounge");

	ui.setupUi(this);

	QObject::connect(ui.updateButton, SIGNAL(clicked()), this, SLOT(checkUpdate())); // change to button in ui.updatePanel
	QObject::connect(ui.helpButton, SIGNAL(toggled(bool)), this, SLOT(changeCursor(bool)));

	helpModeFilter = new HelpModeFilter();
}

sourceGameLounge::~sourceGameLounge()
{
	delete helpModeFilter;
	helpModeFilter = NULL;

	updateCheckCommunicator = true;

	if (cycleMasterThread.joinable())
		cycleMasterThread.join();
	if (cycleSlaveThread.joinable())
		cycleSlaveThread.join();
}

void sourceGameLounge::checkUpdate()
{
	if (cycleMasterThread.joinable())
		cycleMasterThread.join();

	setChecked(false);

	std::thread swapThread(&sourceGameLounge::cycleSlave, this);
	std::swap(swapThread, cycleSlaveThread);
	swapThread = std::thread(&sourceGameLounge::cycleMaster, this);
	std::swap(swapThread, cycleMasterThread);
}

void sourceGameLounge::changeCursor(bool checked)
{
	if (checked)
	{
		QWhatsThis::enterWhatsThisMode();

		(*g_a).installEventFilter(helpModeFilter);
	}
	else
	{
		(*g_a).removeEventFilter(helpModeFilter);

		QWhatsThis::leaveWhatsThisMode();
	}
}

void sourceGameLounge::setChecked(bool bChecked)
{
	std::string checkedString;

	if (bChecked)
		checkedString = std::string("ingChecked");
	else
		checkedString = std::string("e");

	ui.updateButton->setStyleSheet(QLatin1String(
		"QPushButton {\n"
		"    background-color: rgba(6, 6, 6, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(checkedString.c_str()) + QLatin1String("IconDisabled.png);\n"
		"}\n"
		"\n"
		"QPushButton:hover {\n"
		"    background-color: rgba(34, 34, 34, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(checkedString.c_str()) + QLatin1String("Icon.png);\n"
		"}\n"
		"\n"
		"QPushButton:checked {\n"
		"    background-color: rgba(34, 34, 34, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(checkedString.c_str()) + QLatin1String("Icon.png);\n"
		"}"));
}

void sourceGameLounge::cycleMaster()
{
	// Check for update here

	updateCheckCommunicator = true;

	while (true)
	{
		if (updateCheckCommunicator == false)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	if (cycleSlaveThread.joinable())
		cycleSlaveThread.join();

	setChecked(true);

	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	ui.updateButton->repaint();
}

void sourceGameLounge::cycleSlave()
{
	while (true)
	{

		if (updateCheckCommunicator == true)
			break;

		for (int iconNumber = 0; iconNumber <= 5; ++iconNumber)
		{

			ui.updateButton->setStyleSheet(QLatin1String(
				"QPushButton {\n"
				"    background-color: rgba(6, 6, 6, 100%);\n"
				"    border-image: url(:/sourceGameLounge/images/updatingIcon") + QLatin1String(std::to_string(iconNumber).c_str()) + QLatin1String("Disabled.png);\n"
				"}\n"
				"\n"
				"QPushButton:hover {\n"
				"    background-color: rgba(34, 34, 34, 100%);\n"
				"    border-image: url(:/sourceGameLounge/images/updatingIcon") + QLatin1String(std::to_string(iconNumber).c_str()) + QLatin1String(".png);\n"
				"}\n"
				"\n"
				"QPushButton:checked {\n"
				"    background-color: rgba(34, 34, 34, 100%);\n"
				"    border-image: url(:/sourceGameLounge/images/updatingIcon") + QLatin1String(std::to_string(iconNumber).c_str()) + QLatin1String(".png);\n"
				"}"));

			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}

	updateCheckCommunicator = false;
}

bool HelpModeFilter::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == (*g_w).ui.helpButton && event->type() == QEvent::MouseButtonPress)
	{
		(*g_w).ui.helpButton->toggle();
		return true;
	}
	else if (event->type() == QEvent::LeaveWhatsThisMode)
	{
		(*g_w).ui.helpButton->toggle();
		return true;
	}

	return QObject::eventFilter(obj, event);
}

} // namespace sGL

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError)
{
	MessageBoxW(NULL, pwszError, L"sourceGameLounge error:", MB_ICONERROR | MB_OK);
}

extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError)
{
	MessageBoxA(NULL, pszError, "sourceGameLounge error:", MB_ICONERROR | MB_OK);
}