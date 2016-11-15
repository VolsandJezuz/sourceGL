#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#define STRICT
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501
#define WIN32_LEAN_AND_MEAN

#include "ui_sourceGameLounge.h"
#include "config.h"
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <atomic>
#include <thread>
#include <Windows.h>

namespace sGL {

class sourceGameLounge : public QMainWindow // SOURCEGAMELOUNGE_API if need access to main window for plugins
{
	Q_OBJECT

friend class HelpModeFilter;

public:
	sourceGameLounge(QWidget *parent = 0);
	~sourceGameLounge();

private slots:
	void checkUpdate();
	void changeCursor(bool checked = false);

private:
	void setChecked(bool bChecked);
	void cycleMaster();
	void cycleSlave();

	Ui::sourceGameLoungeClass ui;
	HelpModeFilter* helpModeFilter;
	std::atomic<bool> updateCheckCommunicator;
	std::thread cycleMasterThread;
	std::thread cycleSlaveThread;
};

class HelpModeFilter : public QObject
{
	Q_OBJECT

public:
	HelpModeFilter() : QObject() {};
	~HelpModeFilter() = default;
	bool eventFilter(QObject *obj, QEvent*event);
};

} // namespace sGL

extern QApplication* g_a;
extern sGL::sourceGameLounge* g_w;

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError);
extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError);

#endif // SOURCEGAMELOUNGE_H