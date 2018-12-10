#include "Command.h"
#include "MainWindow.h"
#include "MergeHtmlDialog.h"
#include "SettingDialog.h"
#include "GenerateHtml.h"
#include "mutex.h"
#include "html.h"
#include "time_string.h"
#include "log_file.h"

#ifndef _WIN32
#include "Resources/app_ico_xpm.xpm"
#endif

#include <ctime>
#include <cstdlib>
#include <utility>
#include <thread>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iomanip>
#include <wx/dnd.h>
#include <wx/tokenzr.h> 
#include <wx/filename.h>

class FileDropTarget : public wxFileDropTarget
{
public:

	FileDropTarget(MainWindow* w) : owner(w) {}
	bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& paths)
	{
		if (owner->mStage != Stage::PREPARATION) {
			return true;
		}

		wxArrayString p;
		const auto count = paths.Count();
		for (size_t n = 0U; n < count; ++n) {
			const auto& fileName = paths[n];

			// 拡張子を見て棋譜ファイルかどうか確認する
			const auto ext = wxFileName(fileName).GetExt().Lower();
			if (ext == wxT("sgf")
				|| ext == wxT("gib")
				|| ext == wxT("ngf")
				|| ext == wxT("ugf")) {
				p.Add(fileName);
			}
		}

		owner->Appendjobs(p);
		return true;
	}

	MainWindow* owner;
};

MainWindow::MainWindow(const wxString& title)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 640))
{
	mSetting.load();

#ifdef _DEBUG
	Log.open("log.txt");
#else
	if (mSetting.logging) {
		Log.open("log.txt");
	}
#endif

	SetIcon(wxICON(app_ico));

	// Windows 10で表示フォントをメイリオに変更している場合
	// 以下の方法だとメイリオが使用されるが視認性が低いのでやめた
	// auto defaultFont = GetFont();
	// mFont = wxFont(12, defaultFont.GetFamily(), defaultFont.GetStyle(), defaultFont.GetWeight());
	// 以下の方法だと游ゴシックが使用される
	mFont = wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	SetFont(mFont);

	MakeMenuBar();
	auto panel = new wxPanel(this);
	auto vbox = new wxBoxSizer(wxVERTICAL);
	MakeRemainingTimeText(panel, vbox);
	MakeListCtrl(panel, vbox);
	MakeButtons(panel, vbox);
	panel->SetSizer(vbox);

	auto vboxMain = new wxBoxSizer(wxVERTICAL);
	vboxMain->Add(panel, 1, wxEXPAND);
	SetSizer(vboxMain);

	mJobList.clear();
	mTimer.SetOwner(this);
	mSearchSpeed.clear(mSetting.searchSpeed);
	shouldStopGenerating.store(false);
	mOutputPath.clear();

	// ファイルドロップの設定
	SetDropTarget(new FileDropTarget(this));

	// 一部のメニューとボタンを無効化
	SwitchStage(Stage::PREPARATION);

	// ファイルメニュー
	Bind(wxEVT_MENU, &MainWindow::OnAppend, this, CMD_APPEND);
	Bind(wxEVT_MENU, &MainWindow::OnDelete, this, CMD_DELETE);
	Bind(wxEVT_MENU, &MainWindow::OnClear, this, CMD_CLEAR);
	Bind(wxEVT_MENU, &MainWindow::OnExit, this, CMD_EXIT);

	// 問題メニュー
	Bind(wxEVT_MENU, &MainWindow::OnGenerate, this, CMD_GENERATE);
	Bind(wxEVT_MENU, &MainWindow::OnStop, this, CMD_STOP);
	Bind(wxEVT_MENU, &MainWindow::OnSetting, this, CMD_SETTING);

	// HTMLメニュー
	Bind(wxEVT_MENU, &MainWindow::OnMergeHtmls, this, CMD_MERGE_HTMLS);
	Bind(wxEVT_MENU, &MainWindow::OnSepareteHtml, this, CMD_SEPARATE_HTML);
	Bind(wxEVT_MENU, &MainWindow::OnSeparateHtmlByProblemColor, this, CMD_SEPARATE_HTML_BY_PROBLEM_COLOR);

	// ヘルプメニュー
	Bind(wxEVT_MENU, &MainWindow::OnUserGuide, this, CMD_USER_GUIDE);
	Bind(wxEVT_MENU, &MainWindow::OnLicense, this, CMD_LICENSE);

	// ボタン
	Bind(wxEVT_BUTTON, &MainWindow::OnAppend, this, CMD_APPEND);
	Bind(wxEVT_BUTTON, &MainWindow::OnDelete, this, CMD_DELETE);
	Bind(wxEVT_BUTTON, &MainWindow::OnClear, this, CMD_CLEAR);
	Bind(wxEVT_BUTTON, &MainWindow::OnGenerate, this, CMD_GENERATE);
	Bind(wxEVT_BUTTON, &MainWindow::OnStop, this, CMD_STOP);

	// タイマー
	Bind(wxEVT_TIMER, &MainWindow::OnTimer, this);

	// Generatorの初期化失敗/中断/終了時
	Bind(wxEVT_THREAD, &MainWindow::OnGenerationFailed, this, CMD_GENERATION_FAILED);
	Bind(wxEVT_THREAD, &MainWindow::OnGenerationStopped, this, CMD_GENERATION_CANCELLED);
	Bind(wxEVT_THREAD, &MainWindow::OnGenerationCompleted, this, CMD_GENERATION_COMPLETED);

	// HTMLの保存に失敗
	Bind(wxEVT_THREAD, &MainWindow::OnSaveError, this, CMD_SAVE_ERROR);

	// 終了処理
	Bind(wxEVT_CLOSE_WINDOW, &MainWindow::OnClose, this);

}	

MainWindow::~MainWindow()
{
}

void MainWindow::MakeMenuBar()
{
	auto menubar = new wxMenuBar();
	MakeFileMenu(menubar);
	MakeProblemMenu(menubar);
	MakeHtmlMenu(menubar);
	MakeHelpMenu(menubar);
	SetMenuBar(menubar);
}

void MainWindow::MakeFileMenu(wxMenuBar* menubar)
{
	mFileMenu = new wxMenu();

	mFileMenu->Append(CMD_APPEND, _("Append"));// wxT("追加")
	mFileMenu->Append(CMD_DELETE, _("Delete"));// wxT("削除")
	mFileMenu->Append(CMD_CLEAR, _("Clear"));// wxT("クリア")
	mFileMenu->AppendSeparator();
	mFileMenu->Append(CMD_EXIT, _("Exit"));// wxT("終了")

	menubar->Append(mFileMenu, _("File"));// wxT("ファイル")
}

void MainWindow::MakeProblemMenu(wxMenuBar* menubar)
{
	mProblemMenu = new wxMenu();

	mProblemMenu->Append(CMD_GENERATE, _("Generate"));// wxT("作成")
	mProblemMenu->Append(CMD_STOP, _("Stop"));// wxT("中止")

	mProblemMenu->AppendSeparator();
	mProblemMenu->Append(CMD_SETTING, _("Settings"));// wxT("設定")

	menubar->Append(mProblemMenu, _("Problem"));// wxT("問題")
}

void MainWindow::MakeHtmlMenu(wxMenuBar* menubar)
{
	auto mHtmlMenu = new wxMenu();
	mHtmlMenu->Append(CMD_MERGE_HTMLS, _("Merge"));// wxT("統合")
	mHtmlMenu->Append(CMD_SEPARATE_HTML, _("Separate"));// wxT("一問ごとに分割")
	mHtmlMenu->Append(CMD_SEPARATE_HTML_BY_PROBLEM_COLOR, _("Separate by color"));// wxT("黒番と白番で分割")

	menubar->Append(mHtmlMenu, _("HTML"));// wxT("HTML")
}

void MainWindow::MakeHelpMenu(wxMenuBar* menubar)
{
	auto helpMenu = new wxMenu();
	helpMenu->Append(CMD_USER_GUIDE, _("User guide"));// wxT("ユーザーガイド")

	helpMenu->AppendSeparator();
	helpMenu->Append(CMD_LICENSE, _("License"));// wxT("ライセンス情報")

	menubar->Append(helpMenu, _("Help"));// wxT("ヘルプ")
}

void MainWindow::MakeRemainingTimeText(wxPanel* panel, wxBoxSizer* vbox)
{
	mRemainingTime = new wxStaticText(panel, wxID_ANY, wxT("----------------------------------------"),
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE | wxALIGN_RIGHT);
	mRemainingTime->SetLabel(wxT(""));
	vbox->Add(mRemainingTime, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 8);
}

void MainWindow::MakeListCtrl(wxPanel* panel, wxBoxSizer* vbox)
{
	mListCtrl = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);

	mListCtrl->InsertColumn(0, _("File"), 0, 600);// wxT("ファイル")
	mListCtrl->InsertColumn(1, _("Progress"), 0, 140);// wxT("進捗率")

	vbox->Add(mListCtrl, 1, wxEXPAND | wxALL, 8);
}

void MainWindow::MakeButtons(wxPanel* panel, wxBoxSizer* vbox)
{
	btAppend = new wxButton(panel, CMD_APPEND, _("Append"), wxDefaultPosition, wxDefaultSize, 0);// wxT("追加")
	btDelete = new wxButton(panel, CMD_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);// wxT("削除")
	btClear = new wxButton(panel, CMD_CLEAR, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);// wxT("クリア")
	btGenerate = new wxButton(panel, CMD_GENERATE, _("Generate"), wxDefaultPosition, wxDefaultSize, 0);// wxT("作成")
	btStop = new wxButton(panel, CMD_STOP, _("Stop"), wxDefaultPosition, wxDefaultSize, 0);// wxT("中止")

	auto hgrid = new wxGridSizer(1, 5, 0, 8);
	hgrid->Add(btAppend, 0, wxEXPAND);
	hgrid->Add(btDelete, 0, wxEXPAND);
	hgrid->Add(btClear, 0, wxEXPAND);
	hgrid->Add(btGenerate, 0, wxEXPAND);
	hgrid->Add(btStop, 0, wxEXPAND);

	vbox->Add(hgrid, 0, wxEXPAND | wxALL, 8);
}

int MainWindow::CalculateRemainingTime()
{
	const auto spv = mSearchSpeed.getSecondPerVisit();
	return mJobList.calculateRemainingTime(mSetting, spv);
}

void MainWindow::ShowRemainingTime(int time)
{
	if (time <= 0) {
		mRemainingTime->SetLabel(wxT(""));
		return;
	}

	auto tokens = wxStringTokenize(_("Time remaining: ;:;:;"), wxT(";"));// wxT("残り ;時間 ;分 ;秒")
	while (tokens.Count() < 4U) {
		tokens.Add(wxT(""));
	}

	wxString s;
	const int hour = time / 3600;
	const int minute = (time - hour * 3600) / 60;
	const int second = time - (hour * 3600 + minute * 60);
	s = tokens[0];
	if (tokens[1] == wxT(":") && tokens[2] == wxT(":")) {
		s += wxString::Format(wxT("%02d:%02d:%02d"), hour, minute, second);
		s += tokens[3];
	} else {
		if (hour > 0) {
			s += wxString::Format(wxT("%d"), hour);
			s += tokens[1];
		}
		if (minute > 0) {
			s += wxString::Format(wxT("%d"), minute);
			s += tokens[2];
		}
		if (second > 0) {
			s += wxString::Format(wxT("%d"), second);
			s += tokens[3];
		}
	}
	mRemainingTime->SetLabel(s);
}

void MainWindow::UpdateMenuAndButtonState()
{
	if (mJobList.jobs.empty()) {
		mFileMenu->Enable(CMD_DELETE, false);
		mFileMenu->Enable(CMD_CLEAR, false);
		mProblemMenu->Enable(CMD_GENERATE, false);
		btDelete->Enable(false);
		btClear->Enable(false);
		btGenerate->Enable(false);
	} else {
		mFileMenu->Enable(CMD_DELETE, true);
		mFileMenu->Enable(CMD_CLEAR, true);
		mProblemMenu->Enable(CMD_GENERATE, true);
		btDelete->Enable(true);
		btClear->Enable(true);
		btGenerate->Enable(true);
	}
}

void MainWindow::SwitchStage(Stage stage)
{
	mStage = stage;
	if (mStage == Stage::PREPARATION) {
		mFileMenu->Enable(CMD_APPEND, true);
		mProblemMenu->Enable(CMD_SETTING, true);
		mProblemMenu->Enable(CMD_STOP, false);
		btAppend->Enable(true);
		btStop->Enable(false);

		// ファイルリストが空でなければクリアボタンと作成ボタンを有効にする
		UpdateMenuAndButtonState();
	} else if (mStage == Stage::GENERATION) {
		mFileMenu->Enable(CMD_APPEND, false);
		mFileMenu->Enable(CMD_DELETE, false);
		mFileMenu->Enable(CMD_CLEAR, false);
		mProblemMenu->Enable(CMD_GENERATE, false);
		mProblemMenu->Enable(CMD_STOP, true);
		mProblemMenu->Enable(CMD_SETTING, false);
		btAppend->Enable(false);
		btDelete->Enable(false);
		btClear->Enable(false);
		btGenerate->Enable(false);
		btStop->Enable(true);
	} else if (mStage == Stage::CANCELING) {
		mFileMenu->Enable(CMD_APPEND, false);
		mFileMenu->Enable(CMD_DELETE, false);
		mFileMenu->Enable(CMD_CLEAR, false);
		mProblemMenu->Enable(CMD_GENERATE, false);
		mProblemMenu->Enable(CMD_STOP, false);
		mProblemMenu->Enable(CMD_SETTING, false);
		btAppend->Enable(false);
		btDelete->Enable(false);
		btClear->Enable(false);
		btGenerate->Enable(false);
		btStop->Enable(false);
	}
}

void MainWindow::AppendListItem(const wxString& fileName)
{
	const auto count = mListCtrl->GetItemCount();
	mListCtrl->InsertItem(count, fileName);
}

void MainWindow::UpdateProgressRate(int index, int progressRate)
{
	const auto li = static_cast<long>(index);
	if (progressRate < 0) {
		mListCtrl->SetItem(li, 1, _("skipped"));// wxT("スキップ")
	} else {
		wxString s = wxString::Format(wxT("%d%%"), progressRate);
		mListCtrl->SetItem(li, 1, s);
	}
}

void MainWindow::UpdateListCtrl()
{
	const int size = mJobList.size();
	for (int i = 0; i < size; ++i) {
		if (mJobList.jobs[i].isUnnecessary()) {
			UpdateProgressRate(i, -1);
		} else {
			UpdateProgressRate(i, mJobList.jobs[i].getProgressRate(mSetting));
		}
	}
}

void MainWindow::ShowInformation(const wxString& message) const
{
	wxMessageBox(message, _("Information"), wxICON_INFORMATION | wxOK);// wxT("報告")
}

void MainWindow::ShowErrorMessage(const wxString& formatString, const wxString& s) const
{
	auto tokens = wxStringTokenize(formatString, wxT(";"));

	while (tokens.Count() < 2U) {
		tokens.Add(wxT(""));
	}
	wxMessageBox(tokens[0] + s + tokens[1], _("Error"), wxICON_ERROR | wxOK);// wxT("エラー")
}

void MainWindow::ShowErrorMessage(const wxString& message) const
{
	wxMessageBox(message, _("Error"), wxICON_ERROR | wxOK);// wxT("エラー")
}

bool MainWindow::SetarateHtml(const wxString& fileName) const
{
	// HTMLを読む
	std::ifstream ifs(static_cast<String>(fileName));
	if (!ifs) {
		ShowErrorMessage(_("Unable to open \";\"."), fileName);// wxT("\";\"が開けません。")
		return false;
	}

	// 問題を取り出す
	const std::string html((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	const std::string title = GetHtmlTitle(html);
	const auto problems = GetProblems(html);
	const int size = static_cast<int>(problems.size());

	if (size == 0) {
		ShowErrorMessage(_("Any Go problem isn't included in \";\"."), fileName);// wxT("\";\"の中に問題が存在しません。")
		return false;
	}

	wxString baseName = fileName;
	const auto n = fileName.find_last_of(wxT('.'));
	if (n != wxString::npos) {
		baseName = fileName.SubString(0U, n - 1);
	}

	// 先に作成予定のファイルが存在しないか調べる
	for (int i = 0; i < size; ++i) {
		const auto s = wxString::Format(wxT("_P%03d.html"), i + 1);
		const auto name = baseName + s;
		if (wxFileName::Exists(name)) {
			ShowErrorMessage(_("Unable to separate the file. \";\" is already exists."), name);// wxT("HTMLファイルを分割できません。\n\";\"が既に存在します。")
			return false;
		}
	}

	for (int i = 0; i < size; ++i) {
		const auto s = wxString::Format(wxT("_P%03d.html"), i + 1);
		const auto name = baseName + s;
		
		std::ofstream ofs(static_cast<String>(name));
		if (!ofs) {
			ShowErrorMessage(_("Unable to save \";\"."), name);// wxT("\";\"を保存できません。")

			// 書き込み禁止等で保存できなかったらそこでやめる
			// ファイルを100個に分割するときに保存できないからといって
			// ダイアログが100回出るようなことになるとまずい
			return false;
		}

		std::vector<std::string> p;
		p.push_back(problems[i]);
		const auto h = GetHtml(title + " (" + std::to_string(i) + ")", p, mSetting.boardSize);
		ofs << h;
	}

	return true;
}

bool MainWindow::SeparateHtmlByProblemColor(const wxString& fileName) const
{
	// HTMLを読む
	std::ifstream ifs(static_cast<String>(fileName));
	if (!ifs) {
		ShowErrorMessage(_("Unable to open \";\"."), fileName);// wxT("\";\"が開けません。")
		return false;
	}

	// 問題を取り出す
	const std::string html((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	const std::string title = GetHtmlTitle(html);
	const auto problems = GetProblems(html);
	const int size = static_cast<int>(problems.size());

	if (size == 0) {
		ShowErrorMessage(_("Any Go problem isn't included in \";\"."), fileName);// wxT("\";\"の中に問題が存在しません。")
		return false;
	}

	wxString baseName = fileName;
	const auto n = fileName.find_last_of(wxT('.'));
	if (n != wxString::npos) {
		baseName = fileName.SubString(0U, n - 1);
	}

	// (ここまではSetarateHtmlと同じになる)

	// 先に作成予定のファイルが存在しないか調べる
	for (int i = 0; i < 2; ++i) {
		const auto s = (i == 0) ? wxT("_B.html") : wxT("_W.html");
		const auto name = baseName + s;
		if (wxFileName::Exists(name)) {
			ShowErrorMessage(_("Unable to separate the file. \";\" is already exists."), name);// wxT("HTMLファイルを分割できません。\n\";\"が既に存在します。")
			return false;
		}
	}

	for (int i = 0; i < 2; ++i) {
		const auto s = (i == 0) ? wxT("_B.html") : wxT("_W.html");
		const auto name = baseName + s;
		const Color color = (i == 0) ? Color::BLACK : Color::WHITE;
		std::vector<std::string> v;

		for (const auto& p : problems) {
			if (GetProblemColor(p) == color) {
				v.push_back(p);
			}
		}

		if (!v.empty()) {
			std::ofstream ofs(static_cast<String>(name));
			if (!ofs) {
				ShowErrorMessage(_("Unable to save \";\"."), name);// wxT("\";\"を保存できません。")
				return false;
			}

			const auto h = GetHtml(title + (i == 0 ? " (B)" : " (W)"), v, mSetting.boardSize);
			ofs << h;
		}
	}

	return true;
}

void MainWindow::Appendjobs(const wxArrayString& paths)
{
	const auto count = paths.Count();
	int index = mJobList.size();
	for (size_t n = 0U; n < count; ++n) {
		const auto& path = paths[n];

		// 同じファイルを重複して追加しないようにする
		if (mJobList.include(String(path))) {
			continue;
		}

		// ファイルの種類の判定は内容を確認せずにファイルの拡張子でする
		const auto ext = wxFileName(path).GetExt().Lower();
		FileType type = FileType::SGF;
		if (ext == wxT("sgf")) {
			type = FileType::SGF;
		} else if (ext == wxT("gib")) {
			type = FileType::GIB;
		} else if (ext == wxT("ngf")) {
			type = FileType::NGF;
		} else if (ext == wxT("ugf")) {
			type = FileType::UGF;
		}

		mJobList.appendJob(Job());
		auto& job = mJobList.jobs.back();
		if (job.read(String(path), type, mSetting)) {
			auto name = wxFileName(path).GetFullName();
			AppendListItem(name);

			// 負の値は問題作成の対象外を表す
			int progress = job.player != Player::NONE ? 0 : -1;

			UpdateProgressRate(index, progress);
			++index;
		} else {

			// ファイルの複数選択に失敗して棋譜ファイルではないものを
			// 読み込もうとしている場合が多いと考えられる
			// エラーを表示してもうっとうしいだけだろうから
			// ただリストへの追加を取り消すだけに留める
			mJobList.deleteJob();
		}
	}
	ShowRemainingTime(CalculateRemainingTime());

	UpdateMenuAndButtonState();
}

void MainWindow::OnAppend(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::PREPARATION) {
		return;
	}

	// TODO: 翻訳ここから
	wxFileDialog dialog(this, _("Open Go game files"),// wxT("棋譜ファイルを開く")
		wxT(""), wxT(""),
		_("Go game files (*.sgf;*.gib;*.ngf;*.ugf)|*.sgf;*.gib;*.ngf;*.ugf"),// wxT("棋譜ファイル (*.sgf;*.gib;*.ngf;*.ugf)|*.sgf;*.gib;*.ngf;*.ugf")
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK)
	{
		wxArrayString paths;
		dialog.GetPaths(paths);

		Appendjobs(paths);
	}
}

void MainWindow::OnDelete(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::PREPARATION) {
		return;
	}
	for (int i = mListCtrl->GetItemCount() - 1; i >= 0; --i) {
		if (mListCtrl->GetItemState(i, wxLIST_STATE_SELECTED) != 0) {
			mListCtrl->DeleteItem(i);
			mJobList.deleteJob(i);
		}
	}
	ShowRemainingTime(CalculateRemainingTime());

	UpdateMenuAndButtonState();
}

void MainWindow::OnClear(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::PREPARATION) {
		return;
	}
	for (int i = mListCtrl->GetItemCount() - 1; i >= 0; --i) {
		mListCtrl->DeleteItem(i);
		mJobList.deleteJob(i);
	}
	ShowRemainingTime(CalculateRemainingTime());

	UpdateMenuAndButtonState();
}

void MainWindow::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void MainWindow::OnGenerate(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::PREPARATION) {
		return;
	}

	// 対象ファイルが一つもない時はなにもしない
	bool noJob = true;
	for (const auto& job : mJobList.jobs) {
		if (!job.isUnnecessary()) {
			noJob = false;
			break;
		}
	}
	if (noJob) {
		return;
	}

	// 現在時刻からファイル名/HTMLのタイトル/問題のIDを作成
	const auto timeString = GetTimeString();

	// 初期値として日時をファイル名にして同じ名前のファイルができないようにしておく
	// ただし、生成ボタンを押したときに出力ファイル名を決めてしまっているので
	// 問題生成中にユーザーが同名のファイルを作ると上書きされてしまうことはある
	// しかし、極めて稀だろうから気にしないことにする

	wxFileDialog dialog(this, _("Choose an output file"), // wxT("出力ファイルを選択")
		wxT(""), wxString(timeString.fileName),
		_("HTML files (*.html)|*.html"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT); // wxT("HTML ファイル (*.html)|*.html")
	if (dialog.ShowModal() != wxID_OK) {
		return;
	}

	// エラーメッセージ用にファイル名を保存
	mOutputPath = dialog.GetPath();

	SwitchStage(Stage::GENERATION); // TODO: もっと前の行に移動したほうがいいかも

	// 問題作成前は残り時間を直接推定するが
	// 問題作成中は実測された処理速度と残りの仕事量から予想終了時刻を求め
	// そこから間接的に残り時間を推定する
	mSearchSpeed.clear();
	mJobList.setExpectedEndTime(mSetting, mSearchSpeed.getSecondPerVisit());

	// 問題作成のスレッドは作成したらすぐに切り離す
	// 処理が完了して自然に止まるまで待つか
	// shouldStopGeneratingのフラグを立てる以外に止める手段はない
	shouldStopGenerating.store(false);
	const auto path = static_cast<const String>(mOutputPath);
	std::thread t(GenerateHtml, this, path, timeString.title, timeString.id);
	t.detach();

	// タイマーは残り時間の表示の更新と進捗の表示の更新のためにある
	mTimer.Start(950, true);
}

void MainWindow::OnStop(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::GENERATION) {
		return;
	}
	mTimer.Stop();
	SwitchStage(Stage::CANCELING);
	shouldStopGenerating.store(true);
}

void MainWindow::OnSetting(wxCommandEvent& WXUNUSED(event))
{
	if (mStage != Stage::PREPARATION) {
		return;
	}
	SettingDialog dialog(_("Settings"), mFont, mSetting); // wxT("設定")
	dialog.ShowModal();
	mJobList.applySetting(mSetting);
	UpdateListCtrl();
	ShowRemainingTime(CalculateRemainingTime());
}

void MainWindow::OnMergeHtmls(wxCommandEvent& WXUNUSED(event))
{
	MergeHtmlDialog dialog(_("Marge HTML files"), mFont); // wxT("HTMLファイルを統合")
	dialog.ShowModal();
}

void MainWindow::OnSepareteHtml(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dialog(this, _("Open HTML files"), // wxT("HTMLファイルを開く")
		wxT(""), wxT(""), _("HTML files (*.html)|*.html"), // wxT("HTML ファイル (*.html)|*.html")
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dialog.GetPaths(paths);
		auto count = paths.Count();
		for (size_t n = 0U; n < count; ++n) {
			const auto& path = paths[n];
			if (!SetarateHtml(path)) {
				return;
			}		
		}
		ShowInformation(_("Each Go problem has copyed onto separate files.")); // wxT("HTMLファイルを一問ごとに分割しました。")
	}
}

void MainWindow::OnSeparateHtmlByProblemColor(wxCommandEvent& WXUNUSED(event))
{
	// 以下OnSepareteHtmlとの違いは2行だけになる

	wxFileDialog dialog(this, _("Open HTML files"), // wxT("HTMLファイルを開く")
		wxT(""), wxT(""), _("HTML files (*.html)|*.html"), // wxT("HTMLファイル (*.html)|*.html")
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dialog.GetPaths(paths);
		auto count = paths.Count();
		for (size_t n = 0U; n < count; ++n) {
			const auto& path = paths[n];
			if (!SeparateHtmlByProblemColor(path)) { // ここ
				return;
			}
		}
		ShowInformation(_("The Go problems which black and white to play has separated into two files.")); // wxT("HTMLファイルを黒番と白番で分割しました。")
	}
}

void MainWindow::OnUserGuide(wxCommandEvent& WXUNUSED(event))
{
#ifdef _WIN32
	wxLaunchDefaultApplication(wxString(".\\Htmls\\") + _("user_guide.html")); // ".\\Htmls\\user_guide_ja.html"
#else
	wxLaunchDefaultApplication(wxString("/usr/share/doc/zproblem/Htmls/") + _("user_guide.html")); // ".\\Htmls\\user_guide_ja.html"
#endif
}

void MainWindow::OnLicense(wxCommandEvent& WXUNUSED(event))
{
#ifdef _WIN32
	wxLaunchDefaultApplication(wxString(".\\Licenses\\") + wxT("license.html"));
#else
	wxLaunchDefaultApplication(wxString("/usr/share/doc/zproblem/Licenses/") + wxT("license.html"));
#endif
}

void MainWindow::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	if (mStage != Stage::GENERATION) {
		return;
	}

	// 最小化しているときは表示の更新はしない
	if (IsIconized()) {
		mTimer.Start(-1, true);
		return;
	}

	// 進捗率を更新する
	std::vector< std::pair<int, int> > v;
	{
		std::lock_guard<std::mutex> lock(gMutex);

		// wxWidgetsのマルチスレッド対応が全く信用できないので
		// ミューテックスでガードされている範囲内では
		// wxWidgetsと無関係なインスタンスの参照や関数の呼び出ししか
		// しないようにする
		int size = mJobList.size();
		for (int i = 0; i < size; ++i) {
			auto& job = mJobList.jobs[i];
			if (job.updated) {
				job.updated = false;
				const int rate = job.getProgressRate(mSetting);
				v.push_back({i, rate});
			}
		}
	}

	// unlock後にGUIを更新する
	for (const auto& p : v) {
		UpdateProgressRate(p.first, p.second);
	}

	// 残り時間を更新する
	const auto now = std::chrono::steady_clock::now();
	const auto expectedEndTime = mJobList.getExpectedEndTime();
	if (expectedEndTime > now) {
		auto t = expectedEndTime - now;
		auto s = std::chrono::duration_cast<std::chrono::seconds>(t).count();
		ShowRemainingTime(static_cast<int>(s));
	} else {
		ShowRemainingTime(0);
	}

	// -1は前回と同じ間隔での呼び出しを意味する
	mTimer.Start(-1, true);
}

void MainWindow::OnGenerationFailed(wxThreadEvent& WXUNUSED(event))
{
	if (mStage == Stage::PREPARATION) {
		return;
	}
	mTimer.Stop();
	ShowErrorMessage(_("Unable to execute a Leela Zero.")); // wxT("Leela Zero の実行に失敗しました。")

	// ユーザーが設定を変えてから再度実行する目的で中断した状況も想定して
	// 進捗をゼロに戻すだけでファイルリストはそのまま残しておく
	mJobList.clearToRegenerate();

	UpdateListCtrl();
	ShowRemainingTime(CalculateRemainingTime());
	SwitchStage(Stage::PREPARATION);
}

void MainWindow::OnGenerationStopped(wxThreadEvent& WXUNUSED(event))
{
	if (mStage == Stage::PREPARATION) {
		return;
	}
	mTimer.Stop();
	ShowInformation(_("Generating Go problems has stopped.")); // wxT("問題作成を中止しました。")

	// ユーザーが設定を変えてから再度実行する目的で中断した状況も想定して
	// 進捗をゼロに戻すだけでファイルリストはそのまま残しておく
	mJobList.clearToRegenerate();

	UpdateListCtrl();
	ShowRemainingTime(CalculateRemainingTime());
	SwitchStage(Stage::PREPARATION);
}

void MainWindow::OnGenerationCompleted(wxThreadEvent& WXUNUSED(event))
{
	// 中止ボタンが押されたのに中止ではなく完了してしまうケースもある

	if (mStage == Stage::PREPARATION) {
		return;
	}
	mTimer.Stop();

	// 進捗率を更新する
	std::vector< std::pair<int, int> > v;
	{
		std::lock_guard<std::mutex> lock(gMutex);

		// wxWidgetsのマルチスレッド対応が全く信用できないので
		// ミューテックスでガードされている範囲内では
		// wxWidgetsと無関係なインスタンスの参照や関数の呼び出ししか
		// しないようにする
		int size = mJobList.size();
		for (int i = 0; i < size; ++i) {
			auto& job = mJobList.jobs[i];
			if (job.updated) {
				job.updated = false;
				const int rate = job.getProgressRate(mSetting);
				v.push_back({i, rate});
			}
		}
	}
	// unlock後にGUIを更新する
	for (const auto& p : v) {
		UpdateProgressRate(p.first, p.second);
	}

	// ダイアログを出すより先に残り時間の表示を消さないと見苦しい
	ShowRemainingTime(0);

	// ミューテックスでガードされている範囲内で
	// wxMeaageBoxを呼んいるShowInformationを呼んではならない
	ShowInformation(_("Generating Go problems has completed."));// wxT("問題作成が終わりました。")

	SwitchStage(Stage::PREPARATION);

	// ユーザーの指示から処理の完了までに長い時間のかかるソフトであるので
	// ダイアログを表示する前にファイルリストを消してしまうと
	// 何をしていたのか思い出せない人もいると想定して最後にクリアする
	wxQueueEvent(this, new wxCommandEvent(wxEVT_MENU, CMD_CLEAR));
}

void MainWindow::OnSaveError(wxThreadEvent& WXUNUSED(event))
{
	if (mStage == Stage::PREPARATION) {
		return;
	}
	mTimer.Stop();
	ShowErrorMessage(wxT("Unable to save \";\"."), mOutputPath);// wxT("\";\"を保存できません。")

	// ユーザーが設定を変えてから再度実行する目的で中断した状況も想定して
	// 進捗をゼロに戻すだけでファイルリストはそのまま残す
	mJobList.clearToRegenerate();

	UpdateListCtrl();
	ShowRemainingTime(CalculateRemainingTime());
	SwitchStage(Stage::PREPARATION);

}

void MainWindow::OnClose(wxCloseEvent& event)
{
	if (event.CanVeto() && mStage == Stage::GENERATION) {

		// 問題の作成中に無理に終了して何らかの不具合が発生するのが怖いので
		// ウインドウを閉じる前にユーザーに処理の中止を要求する
		if (wxMessageBox(_("Do you want to stop generating Go problems?"),// wxT("問題作成を中止しますか?")
			_("Confirmation"), wxICON_WARNING | wxYES_NO) == wxYES) {// wxT("確認")

			// このダイアログがでている間に処理が完了しても
			// 正しく動くようにイベントキューを介してキャンセルする
			wxQueueEvent(this, new wxCommandEvent(wxEVT_MENU, CMD_STOP));
		}
		event.Veto();
	} else if (event.CanVeto() && mStage == Stage::CANCELING) {

		// 中止を待たずに終了したい人には自己責任で強制終了してもらう
		if (wxMessageBox(_("Do you want to force-quit this application?"),// wxT("強制終了しますか?")
			_("Confirmation"), wxICON_WARNING | wxYES_NO) == wxYES) {// wxT("確認")
			event.Skip();
		} else {
			event.Veto();
		}
	} else {

		// タイマーは止まっているはずだが念のために止める
		mTimer.Stop();

		// 終了時に記録されている探索の速度を次回の実行時に
		// 処理の残り時間を予測するための初期値として保存する
		mSetting.searchSpeed = mSearchSpeed.getSecondPerVisit();
		mSetting.save();
		event.Skip();
	}

	Log.close();
}
