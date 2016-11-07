#include "sourceGameLounge.h"
#include "commonDLL.h"

namespace sGL {

sourceGameLounge::sourceGameLounge(QWidget *parent) : QMainWindow(parent)
{
	commondll::commonDLL::instance().addName("sourceGameLounge");

	ui.setupUi(this);

	ui.helpOnIcon->hide();
	ui.updatingIcon5->hide();
	ui.updatingIcon4->hide();
	ui.updatingIcon3->hide();
	ui.updatingIcon2->hide();
	ui.updatingIcon1->hide();
	ui.updatingIcon0->hide();
	ui.updateCheckedIcon->hide();
}

} // namespace sGL

extern "C" SOURCEGAMELOUNGE_API void displayError(LPCWSTR pwszError)
{
	MessageBoxW(NULL, pwszError, L"sourceGL error:", MB_ICONERROR | MB_OK);
}

extern "C" SOURCEGAMELOUNGE_API void displayErrorA(LPCSTR pszError)
{
	MessageBoxA(NULL, pszError, "sourceGL error:", MB_ICONERROR | MB_OK);
}