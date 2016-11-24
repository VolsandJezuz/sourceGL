#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501

#include "sourceGameLounge.h"
#include "commonDLL.h"
#include <QtWidgets/QWhatsThis>
#include <chrono>
#include <string>

namespace sgl {

SourceGameLounge::SourceGameLounge(QApplication* pApplication, QWidget* parent) : QMainWindow(parent)
{
	commondll::CommonDLL::instance().addName("SourceGameLounge");

	this->pApplication = pApplication;

	ui.setupUi(this);

	pStackedLayout = new QStackedLayout;

	pStackedLayout->addWidget(ui.gamesPane);
	pStackedLayout->addWidget(ui.gameSettingsPane);
	pStackedLayout->addWidget(ui.programSettingsPane);
	pStackedLayout->addWidget(ui.updatePane);
	pStackedLayout->addWidget(ui.aboutPane);

	//function to addWidget and map/connect plugin Panes

	pSignalMapper = new QSignalMapper;

	connect(ui.gamesButton, SIGNAL(clicked()), pSignalMapper, SLOT(map()));
	pSignalMapper->setMapping(ui.gamesButton, ui.gamesPane);

	connect(ui.gameSettingsButton, SIGNAL(clicked()), pSignalMapper, SLOT(map()));
	pSignalMapper->setMapping(ui.gameSettingsButton, ui.gameSettingsPane);

	connect(ui.programSettingsButton, SIGNAL(clicked()), pSignalMapper, SLOT(map()));
	pSignalMapper->setMapping(ui.programSettingsButton, ui.programSettingsPane);

	connect(ui.updateButton, SIGNAL(clicked()), pSignalMapper, SLOT(map()));
	pSignalMapper->setMapping(ui.updateButton, ui.updatePane);

	connect(ui.aboutButton, SIGNAL(clicked()), pSignalMapper, SLOT(map()));
	pSignalMapper->setMapping(ui.aboutButton, ui.aboutPane);

	QObject::connect(pSignalMapper, SIGNAL(mapped(QWidget*)), pStackedLayout, SLOT(setCurrentWidget(QWidget*)));
	QObject::connect(ui.updateButton, SIGNAL(clicked()), this, SLOT(checkUpdate())); // change to button in ui.updatePanel
	QObject::connect(ui.helpButton, SIGNAL(toggled(bool)), this, SLOT(changeWhatsThisMode(bool)));

	pHelpModeFilter = new HelpModeFilter();
}

SourceGameLounge::~SourceGameLounge()
{
	delete pHelpModeFilter;
	pHelpModeFilter = NULL;

	delete pSignalMapper;
	pSignalMapper = NULL;

	delete pStackedLayout;
	pStackedLayout = NULL;

	updateCheckCommunicator = true;

	if (cycleMasterThread.joinable())
		cycleMasterThread.join();

	if (cycleSlaveThread.joinable())
		cycleSlaveThread.join();
}

void SourceGameLounge::checkUpdate()
{
	if (cycleMasterThread.joinable())
		cycleMasterThread.join();

	setChecked(false);

	std::thread swapThread(&SourceGameLounge::cycleSlave, this);
	std::swap(swapThread, cycleSlaveThread);
	swapThread = std::thread(&SourceGameLounge::cycleMaster, this);
	std::swap(swapThread, cycleMasterThread);
}

void SourceGameLounge::changeWhatsThisMode(bool checked) const
{
	if (checked)
	{
		QWhatsThis::enterWhatsThisMode();

		pApplication->installEventFilter(pHelpModeFilter);
	}
	else
	{
		pApplication->removeEventFilter(pHelpModeFilter);

		QWhatsThis::leaveWhatsThisMode();
	}
}

void SourceGameLounge::setChecked(bool bChecked)
{
	std::string checkedString;

	if (bChecked)
		checkedString = std::string("ingChecked");
	else
		checkedString = std::string("e");

	updateStyleSheet((checkedString + "Icon").c_str());
}

void SourceGameLounge::updateStyleSheet(const CHAR* iconSpecifier)
{
	ui.updateButton->setStyleSheet(QLatin1String(
		"QPushButton {\n"
		"    background-color: rgba(6, 6, 6, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(iconSpecifier) + QLatin1String("Disabled.png);\n"
		"}\n"
		"\n"
		"QPushButton:hover {\n"
		"    background-color: rgba(34, 34, 34, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(iconSpecifier) + QLatin1String(".png);\n"
		"}\n"
		"\n"
		"QPushButton:checked {\n"
		"    background-color: rgba(34, 34, 34, 100%);\n"
		"    border-image: url(:/sourceGameLounge/images/updat") + QLatin1String(iconSpecifier) + QLatin1String(".png);\n"
		"}"));
}

void SourceGameLounge::cycleMaster()
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

void SourceGameLounge::cycleSlave()
{
	while (true)
	{

		if (updateCheckCommunicator == true)
			break;

		for (int iconNumber = 0; iconNumber <= 5; ++iconNumber)
		{
			updateStyleSheet(("ingIcon" + std::to_string(iconNumber)).c_str());

			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
	}

	updateCheckCommunicator = false;
}

bool HelpModeFilter::eventFilter(QObject *object, QEvent *event)
{
	if (object == g_pWindow->ui.helpButton && event->type() == QEvent::MouseButtonPress)
	{
		g_pWindow->ui.helpButton->toggle();

		return true;
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		QEvent qEvent = QEvent(QEvent::Leave);
		QApplication::sendEvent(object, &qEvent);
	}
	else if (event->type() == QEvent::LeaveWhatsThisMode)
		g_pWindow->ui.helpButton->toggle();

	return QObject::eventFilter(object, event);
}

} // namespace sgl

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError)
{
	MessageBoxW(NULL, pwszError, L"sourceGameLounge error:", MB_ICONERROR | MB_OK);
}

extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError)
{
	MessageBoxA(NULL, pszError, "sourceGameLounge error:", MB_ICONERROR | MB_OK);
}