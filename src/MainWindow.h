#pragma once

#include "setting.h"
#include "job_list.h"
#include "search_speed.h"

#include <set>
#include <memory>
#include <chrono>
#include <atomic>
#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/listctrl.h>
#include <wx/thread.h>

enum class Stage : int
{
	PREPARATION,
	GENERATION,
	CANCELING
};

class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString& title);
	~MainWindow();

private:

	// GUI作成
	void MakeMenuBar();
	void MakeFileMenu(wxMenuBar* menubar);
	void MakeProblemMenu(wxMenuBar* menubar);
	void MakeHtmlMenu(wxMenuBar* menubar);
	void MakeHelpMenu(wxMenuBar* menubar);
	void MakeRemainingTimeText(wxPanel* panel, wxBoxSizer* vbox);
	void MakeListCtrl(wxPanel* panel, wxBoxSizer* vbox);
	void MakeButtons(wxPanel* panel, wxBoxSizer* vbox);

	// 残り時間の表示
	int CalculateRemainingTime();
	void ShowRemainingTime(int time);

	// GUI更新
	void UpdateMenuAndButtonState();
	void SwitchStage(Stage stage);

	// ファイルリスト操作
	void AppendListItem(const wxString& fileName);
	void UpdateProgressRate(int index, int progressRate);
	void UpdateListCtrl();

	// メッセージダイアログ
	void ShowInformation(const wxString& message) const;
	void ShowErrorMessage(const wxString& formatString, const wxString& fileName) const;
	void ShowErrorMessage(const wxString& message) const;

	// HTML操作
	bool SetarateHtml(const wxString& fileName) const;
	bool SeparateHtmlByProblemColor(const wxString& fileName) const;

	// ファイルメニュー
	void Appendjobs(const wxArrayString& paths);
	void OnAppend(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);

	// 問題メニュー
	void OnGenerate(wxCommandEvent& event);
	void OnStop(wxCommandEvent& event);
	void OnSetting(wxCommandEvent& event);

	// HTMLメニュー
	void OnMergeHtmls(wxCommandEvent& event);
	void OnSepareteHtml(wxCommandEvent& event);
	void OnSeparateHtmlByProblemColor(wxCommandEvent& event);

	// ヘルプメニュー
	void OnUserGuide(wxCommandEvent& event);
	void OnLicense(wxCommandEvent& event);

	// タイマー
	void OnTimer(wxTimerEvent& event);

	// Generatorの初期化失敗/中断/終了時
	void OnGenerationFailed(wxThreadEvent& event);
	void OnGenerationStopped(wxThreadEvent& event);
	void OnGenerationCompleted(wxThreadEvent& event);

	// HTMLの保存に失敗
	void OnSaveError(wxThreadEvent& event);

	// 終了処理
	void OnClose(wxCloseEvent& event);

private:
	Setting mSetting;
	Stage mStage;
	wxFont mFont;
	wxStaticText* mRemainingTime;
	wxMenu* mFileMenu;
	wxMenu* mProblemMenu;
	wxListCtrl* mListCtrl;
	wxButton* btAppend;
	wxButton* btDelete;
	wxButton* btClear;
	wxButton* btGenerate;
	wxButton* btStop;
	JobList mJobList;
	wxTimer mTimer;	
	SearchSpeed mSearchSpeed;
	std::atomic<bool> shouldStopGenerating;
	wxString mOutputPath;

	friend class FileDropTarget;
	friend class ProblemGenerator;
	friend void GenerateHtml(MainWindow* window, const String& path,
		const std::string& title, const std::string& id);
};
