#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "ui_sourceGameLounge.h"
#include "config.h"
#include <QtCore/QSignalMapper>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStackedLayout>
#include <atomic>
#include <thread>
#include <Windows.h>

namespace sgl {

class SourceGameLounge : public QMainWindow // SOURCEGAMELOUNGE_API if need access to main window for plugins
{
	Q_OBJECT

friend class HelpModeFilter;

public:
	SourceGameLounge(QWidget *parent = 0);
	~SourceGameLounge();

	QStackedLayout* pStackedLayout;

private slots:
	void checkUpdate();
	void changeWhatsThisMode(bool checked = false) const;

private:
	void setChecked(bool bChecked);
	void cycleMaster();
	void cycleSlave();

	Ui::SourceGameLoungeClass ui;
	QSignalMapper* pSignalMapper;
	HelpModeFilter* pHelpModeFilter;
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
	bool eventFilter(QObject* object, QEvent* event);
};

} // namespace sgl

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError);
extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError);
extern QApplication* g_pApplication;
extern sgl::SourceGameLounge* g_pWindow;

#endif // SOURCEGAMELOUNGE_H