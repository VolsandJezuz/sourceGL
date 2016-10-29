#ifndef SOURCEGAMELOUNGE_H
#define SOURCEGAMELOUNGE_H

#include <QtWidgets/QMainWindow>
#include "ui_sourceGameLounge.h"

#ifdef SOURCEGAMELOUNGE_EXPORTS
#define SOURCEGAMELOUNGE_API __declspec(dllexport)
#else
#define SOURCEGAMELOUNGE_API __declspec(dllimport)
#endif

class SOURCEGAMELOUNGE_API sourceGameLounge : public QMainWindow
{
	Q_OBJECT

public:
	sourceGameLounge(QWidget *parent = 0);
	~sourceGameLounge();

private:
	Ui::sourceGameLoungeClass ui;
};

#endif