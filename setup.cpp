#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "setup.h"
#include "config.h"
#include "sourceGameLounge.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <codecvt>
#include <filesystem>
#include <locale>
#include <Windows.h>

namespace sgl {

Setup::Setup(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	QObject::connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
}

SetupPre::SetupPre()
{
	bSetupDelete = false;
}

SetupPre::~SetupPre()
{
	if (bSetupDelete)
	{
		delete pSetup;
		pSetup = NULL;
	}
}

bool SetupPre::initialSetup()
{
	if (findSteamDir())
	{
		std::vector<std::wstring> steamIDs = enumSteamIDs();

		bool ret = Config::instance().steam32ID != 0;
		if (ret)
			ret = std::find(steamIDs.begin(), steamIDs.end(), std::to_wstring(Config::instance().steam32ID)) != steamIDs.end();

		if (!ret)
		{
			pSetup = new Setup();
			bSetupDelete = true;

			for (std::wstring steamID : steamIDs)
				getPlayerSummary(steamID, pSetup);

			return false;
		}
	}
	else
	{
		Config::instance().steam32ID = 1;
		displayError(L"Steam installation was not found, so games can only be added manually.");
	}

	return true;
}

bool SetupPre::findSteamDir()
{
	HKEY hKey = NULL;
	DWORD keyType = REG_SZ;
	WCHAR keyData[MAX_PATH] = {0};
	DWORD keyDataSize = sizeof(keyData);

	LONG ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
	if (ret == ERROR_SUCCESS)
	{
		ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, (LPBYTE)keyData, &keyDataSize);
		if (ret == ERROR_SUCCESS)
			steamDir = keyData;

		RegCloseKey(hKey);
	}

	if (ret == ERROR_FILE_NOT_FOUND)
	{
		ret = RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Valve\\Steam", 0, KEY_QUERY_VALUE, &hKey);
		if (ret == ERROR_SUCCESS)
		{
			ret = RegQueryValueExW(hKey, L"InstallPath", NULL, &keyType, (LPBYTE)keyData, &keyDataSize);
			if (ret == ERROR_SUCCESS)
				steamDir = keyData;

			RegCloseKey(hKey);
		}

		if (ret != ERROR_SUCCESS)
			return false;
	}

	return true;
}

std::vector<std::wstring> SetupPre::enumSteamIDs()
{
	std::vector<std::wstring> steamIDs;
	WIN32_FIND_DATA fileData;

	HANDLE fileHandle = FindFirstFileW((steamDir + L"\\userdata\\*").c_str(), &fileData);
	if (fileHandle != (void*)ERROR_INVALID_HANDLE && fileHandle != (void*)ERROR_FILE_NOT_FOUND)
	{
		do
		{
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && std::wstring(fileData.cFileName) != std::wstring(L".") && std::wstring(fileData.cFileName) != std::wstring(L".."))
				steamIDs.push_back(std::wstring(fileData.cFileName));
		} while (FindNextFileW(fileHandle, &fileData));
	}

	return steamIDs;
}

void SetupPre::getPlayerSummary(std::wstring steamID, Setup* pSetup)
{
	QUrl url(QString("http://localhost:3000/GetPlayerSummary/%1").arg(std::to_string(std::stoll(steamID) + 76561197960265728).c_str()));
	QNetworkRequest networkRequest(url);
	QNetworkReply* pNetworkReply = pSetup->networkAccessManager.get(networkRequest);

	pSetup->eventLoop.exec();

	if (pNetworkReply->error() == QNetworkReply::NoError)
	{
		QString string = (QString)pNetworkReply->readAll();
		makeSteamSetupWidgets(string);
	}
	else
	{
		std::wstring filePath(steamDir + L"\\userdata\\" + steamID + L"\\config\\localconfig.vdf");
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		// and then I figured out localconfig.vdf was not in JSON format...this section not working right now
		/*makeSetupWidgets(converter.to_bytes(filePath).c_str());
		std::string filename = converter.to_bytes(filePath).c_str();
		makeSetupWidgets(filename);*/
	}

	delete pNetworkReply;
	pNetworkReply = NULL;
}

void SetupPre::makeSteamSetupWidgets(QString string)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(string.toUtf8());
	QJsonObject jsonObject = jsonDocument.object();

	// just for demonstration purposes
	displayErrorA(jsonObject["profileurl"].toString().toStdString().c_str());
	jsonObject["avatarmedium"].toString();
	displayErrorA(jsonObject["personaname"].toString().toStdString().c_str());
}

void SetupPre::makeSetupWidgets(const std::string &filename)
{
	std::tr2::sys::path myFile(filename);
	if (std::tr2::sys::exists(myFile))
	{
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(filename, pt);
		displayErrorA(pt.get<std::string>("UserLocalConfigStore.friends.PersonaName").c_str());
	}
}

} // namespace sgl