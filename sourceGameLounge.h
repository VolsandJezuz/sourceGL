#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#define STRICT
#define WINVER 0x0501
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <QtWidgets/QMainWindow>
#include "ui_sourceGameLounge.h"

class sourceGameLounge : public QMainWindow
{
	Q_OBJECT

public:
	sourceGameLounge(QWidget *parent = 0);
	~sourceGameLounge();

private:
	Ui::sourceGameLoungeClass ui;
};

void displayError(LPTSTR pwszError);

#endif