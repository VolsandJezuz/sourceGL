#include "sourceGameLounge.h"
#include "config.h"
#include "wmiEventSink.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	sConfig sC;

	try
	{
		sC.load("config.xml");
	}
	catch (std::exception &e)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, NULL, 0);
		WCHAR *pwszError = new WCHAR[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, pwszError, size_needed);

		displayError(pwszError);

		delete[] pwszError;
	}

	if (!wmiInitialize())
	{
		return true;
	}

	QApplication a(argc, argv);
	sourceGameLounge w;
	w.show();
	int ret = a.exec();

	try
	{
		sC.save("config.xml");
	}
	catch (std::exception &e)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, NULL, 0);
		WCHAR *pwszError = new WCHAR[size_needed];
		MultiByteToWideChar(CP_UTF8, 0, e.what(), -1, pwszError, size_needed);

		displayError(pwszError);

		delete[] pwszError;
	}

	return ret;
}

void displayError(LPTSTR pwszError)
{
	MessageBox(NULL, pwszError, TEXT("sourceGL error:"), MB_ICONERROR | MB_OK);
}