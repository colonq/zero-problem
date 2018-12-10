#include "MainWindow.h"
#include "log_file.h"

#include <wx/snglinst.h>

class App : public wxApp
{
public:
	App() : mLocale(wxLANGUAGE_DEFAULT) {}

	virtual bool OnInit();
	virtual int OnExit();

private:
	wxLocale mLocale;
	wxSingleInstanceChecker* mChecker;
};

bool App::OnInit()
{

	// CPUやGPUのリソースを大量に使うこのソフトの性質上から
	// 二重に起動したいケースはないはずである
	mChecker = new wxSingleInstanceChecker;
	if (mChecker->IsAnotherRunning()) {
		delete mChecker;
		mChecker = nullptr;
		return false;
	}

#ifdef _WIN32
	mLocale.AddCatalogLookupPathPrefix(wxT("Locale\\"));
#endif
	mLocale.AddCatalog(wxT("zproblem"));

	MainWindow* window = new MainWindow(_("Zero Problem"));
	window->Show(true);

	return true;
}

int App::OnExit()
{
	delete mChecker;
	return 0;
}

IMPLEMENT_APP(App)
