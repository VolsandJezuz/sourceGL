#ifndef SETUP_H
#define SETUP_H

#include "ui_setup.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtWidgets/QDialog>
#include <string>
#include <vector>

namespace sgl {

class Setup : public QDialog
{
	Q_OBJECT

public:
	Setup(QWidget *parent = 0);
	~Setup() = default;

	QEventLoop eventLoop;
	QNetworkAccessManager networkAccessManager;

private:
	Ui::Setup ui;
};

class SetupPre
{
public:
	SetupPre();
	~SetupPre();
	bool initialSetup();

	std::wstring steamDir;
	Setup* pSetup;

private:
	bool findSteamDir();
	std::vector<std::wstring> enumSteamIDs();
	void getPlayerSummary(std::wstring steamID, Setup* pSetup);
	void makeSteamSetupWidgets(QString string);
	void makeSetupWidgets(const std::string &filename);

	bool bSetupDelete;
};

} // namespace sgl

#endif // SETUP_H
