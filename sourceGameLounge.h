#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "ui_sourceGameLounge.h"
#include "config.h"
#include <QtWidgets/QMainWindow>
#include <Windows.h>

namespace sGL {

class sourceGameLounge : public QMainWindow // SOURCEGAMELOUNGE_API if need access to main window for plugins
{
	Q_OBJECT

public:
	sourceGameLounge(QWidget *parent = 0);
	~sourceGameLounge() = default;

	std::vector<commondll::plugin*> plugins;

private:
	Ui::sourceGameLoungeClass ui;
};

} // namespace sGL

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError);
extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError);

#endif // SOURCEGAMELOUNGE_H