#include "sourceGameLounge.h"
#include "wmiEventSink.h"
#include <QtWidgets/QApplication>

WNDPROC OldWinProc = NULL;

int main(int argc, char *argv[])
{
	if (!wmiInitialize())
	{
		return true;
	}
	/*sNvAPIObject = new sNvAPI;
	WCHAR pwszError[256];
	if (!sNvAPIObject->Start(pwszError))
	displayError(pwszError);

	if (!sNvAPIObject->SetCoolerLevels(0, 0, 100))
	//displayError(TEXT("OOPS"));
	std::string s = std::to_string(sNvAPIObject->arr_CoolerInfo[0].Coolers[0].currentLevel);
	MessageBoxA(NULL, s.c_str(), "sourceGL error:", MB_ICONERROR | MB_OK);*/

	QApplication a(argc, argv);
	sourceGameLounge w;
	w.show();
	return a.exec();
}

void displayError(LPTSTR pwszError)
{
	MessageBox(NULL, pwszError, TEXT("sourceGL error:"), MB_ICONERROR | MB_OK);
}