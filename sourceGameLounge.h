#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "config.h"
#include "ui_sourceGameLounge.h"
#include <QtCore/QSignalMapper>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStackedLayout>
#include <atomic>
#include <thread>
#include <Windows.h>

namespace sgl {

class SourceGameLounge : public QMainWindow
{
	Q_OBJECT

friend class HelpModeFilter;

public:
	SourceGameLounge(QApplication* pApplication, QWidget* parent = 0);
	~SourceGameLounge();

	QStackedLayout* pStackedLayout;

private slots:
	void checkUpdate();
	void changeWhatsThisMode(bool checked = false) const;

private:
	void setChecked(bool bChecked);
	void updateStyleSheet(const CHAR* iconSpecifier);
	void cycleMaster();
	void cycleSlave();

	QApplication* pApplication;
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
extern sgl::SourceGameLounge* g_pWindow;

#endif // SOURCEGAMELOUNGE_H