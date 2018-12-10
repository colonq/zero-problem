#include "MergeHtmlDialog.h"
#include "Command.h"
#include "string_util.h"
#include "html.h"
#include "time_string.h"

#include <fstream>
#include <iterator>
#include <wx/dnd.h>
#include <wx/tokenzr.h> 
#include <wx/filename.h>

class FileDropTargetForMergeHtmlDialog : public wxFileDropTarget
{
public:

	FileDropTargetForMergeHtmlDialog(MergeHtmlDialog* w) : owner(w) {}
	bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& paths)
	{
		wxArrayString p;
		const auto count = paths.Count();
		for (size_t n = 0U; n < count; ++n) {
			const auto& fileName = paths[n];

			// 拡張子を見てHTMLファイルかどうか確認する
			const auto ext = wxFileName(fileName).GetExt().Lower();
			if (ext == wxT("html")) {
				p.Add(fileName);
			}
		}

		owner->AppendFiles(p);
		return true;
	}

	MergeHtmlDialog* owner;
};

MergeHtmlDialog::MergeHtmlDialog(const wxString& title, const wxFont& font)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(800, 640), wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX)
{
	SetFont(font);

	auto vbox = new wxBoxSizer(wxVERTICAL);

	MakeListBox(vbox);
	MakeButtons(vbox);
	UpdateButtonState();

	SetDropTarget(new FileDropTargetForMergeHtmlDialog(this));

	Bind(wxEVT_BUTTON, &MergeHtmlDialog::OnAppend, this, CMD_MD_APPEND);
	Bind(wxEVT_BUTTON, &MergeHtmlDialog::OnDelete, this, CMD_MD_DELETE);
	Bind(wxEVT_BUTTON, &MergeHtmlDialog::OnClear, this, CMD_MD_CLEAR);
	Bind(wxEVT_BUTTON, &MergeHtmlDialog::OnMerge, this, CMD_MD_MERGE);
	Bind(wxEVT_BUTTON, &MergeHtmlDialog::OnClose, this, CMD_MD_CLOSE);

	SetSizer(vbox);
	Center();
}

void MergeHtmlDialog::MakeListBox(wxBoxSizer* vbox)
{
	lbFileList = new wxListBox(this, wxID_ANY);
	vbox->Add(lbFileList, 1, wxEXPAND | wxALL, 8);
}

void MergeHtmlDialog::MakeButtons(wxBoxSizer* vbox)
{
	auto btAppend = new wxButton(this, CMD_MD_APPEND, _("Append"), wxDefaultPosition, wxDefaultSize, 0);// wxT("追加")
	btDelete = new wxButton(this, CMD_MD_DELETE, _("Delete"), wxDefaultPosition, wxDefaultSize, 0);// wxT("削除")
	btClear = new wxButton(this, CMD_MD_CLEAR, _("Clear"), wxDefaultPosition, wxDefaultSize, 0);// wxT("クリア")
	btMerge = new wxButton(this, CMD_MD_MERGE, _("Merge"), wxDefaultPosition, wxDefaultSize, 0);// wxT("統合")
	auto btClose = new wxButton(this, CMD_MD_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0);// wxT("閉じる")

	auto hgrid = new wxGridSizer(1, 5, 0, 8);
	hgrid->Add(btAppend, 0, wxEXPAND);
	hgrid->Add(btDelete, 0, wxEXPAND);
	hgrid->Add(btClear, 0, wxEXPAND);
	hgrid->Add(btMerge, 0, wxEXPAND);
	hgrid->Add(btClose, 0, wxEXPAND);

	vbox->Add(hgrid, 0, wxEXPAND | wxALL, 8);
}

void MergeHtmlDialog::UpdateButtonState()
{
	if (asFileList.IsEmpty()) {
		btDelete->Enable(false);
		btClear->Enable(false);
	} else {
		btDelete->Enable(true);
		btClear->Enable(true);
	}

	if (asFileList.Count() < 2U) {
		btMerge->Enable(false);
	} else {
		btMerge->Enable(true);
	}
}

void MergeHtmlDialog::ShowErrorMessage(const wxString& formatString, const wxString& s) const
{
	// これはメインウインドウにあるのと同じ関数になっている

	auto tokens = wxStringTokenize(formatString, wxT(";"));

	while (tokens.Count() < 2U) {
		tokens.Add(wxT(""));
	}
	wxMessageBox(tokens[0] + s + tokens[1], _("Error"), wxICON_ERROR | wxOK);// wxT("エラー")
}

void MergeHtmlDialog::ShowInformation(const wxString& message) const
{
	// これはメインウインドウにあるのと同じ関数になっている

	wxMessageBox(message, _("Information"), wxICON_INFORMATION | wxOK);// wxT("報告")
}

void MergeHtmlDialog::ShowErrorMessage(const wxString& message) const
{
	// これはメインウインドウにあるのと同じ関数になっている

	wxMessageBox(message, _("Error"), wxICON_ERROR | wxOK);// wxT("エラー")
}

void MergeHtmlDialog::AppendFiles(const wxArrayString& paths)
{
	const auto count = paths.Count();
	for (size_t n = 0U; n < count; ++n) {

		// 同じパスをリストに入れない
		const auto& path = paths[n];
		const auto c = asFileList.Count();
		size_t m;
		for (m = 0U; m < c; ++m) {
			if (asFileList[m] == path) {
				break;
			}
		}
		if (m == c) {
			asFileList.Add(path);
			auto name = wxFileName(path).GetFullName();
			lbFileList->Append(name);
		}
	}
	UpdateButtonState();
}

void MergeHtmlDialog::OnAppend(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dialog(this, _("Open HTML files"),// wxT("HTMLファイルを開く")
		wxT(""), wxT(""), _("HTML files (*.html)|*.html"),// wxT("HTMLファイル (*.html)|*.html")
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (dialog.ShowModal() == wxID_OK) {
		wxArrayString paths;
		dialog.GetPaths(paths);

		AppendFiles(paths);
	}
}

void MergeHtmlDialog::OnDelete(wxCommandEvent& WXUNUSED(event))
{
	const auto count = static_cast<int>(asFileList.Count());
	for (int i = count - 1; i >= 0; --i) {
		if (lbFileList->IsSelected(i)) {
			auto n = static_cast<size_t>(i);
			lbFileList->Delete(n);
			asFileList.RemoveAt(n);
		}
	}
	UpdateButtonState();
}

void MergeHtmlDialog::OnClear(wxCommandEvent& WXUNUSED(event))
{
	if (asFileList.IsEmpty()) {
		return;
	}

	asFileList.Clear();
	lbFileList->Clear();
	UpdateButtonState();
}

void MergeHtmlDialog::OnMerge(wxCommandEvent& WXUNUSED(event))
{
	if (asFileList.Count() < 2U) {
		return;
	}

	const auto timeString = GetTimeString();

	wxFileDialog dialog(this, _("Choose an output file"),// wxT("出力ファイルを選択")
		wxT(""), wxString(timeString.fileName_M),
		_("HTML files (*.html)|*.html"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);// wxT("HTML ファイル (*.html)|*.html")
	if (dialog.ShowModal() != wxID_OK) {
		return;
	}

	const auto outputPath = dialog.GetPath();

	std::vector<std::string> problems;

	const auto count = asFileList.Count();
	for (size_t n = 0; n < count; ++n) {
		const auto& fileName = asFileList[n];
		std::ifstream ifs(static_cast<String>(fileName));
		if (!ifs) {
			ShowErrorMessage(_("Unable to open \";\"."), fileName);// wxT("\";\"が開けません。")
			return;
		}

		// 問題を取り出してproblemsに追加する
		const std::string h((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		auto p = GetProblems(h);
		problems.insert(problems.end(), p.begin(), p.end());
	}

	// GetHtml内で重複する問題が出力されないように対処される
	const auto html = GetHtml(timeString.title_M, problems, 19);

	// 現在は19路の問題にしか対応していない
	// const auto html9 = GetHtml(timeString.title_M, problems, 9);

	if (html.empty()) {
		ShowErrorMessage(_("Any Go problem isn't included in \";\"."));// wxT("\";\"の中に問題が存在しません。")
		return;
	}

	std::ofstream ofs(static_cast<String>(outputPath));
	if (!ofs) {
		ShowErrorMessage(_("Unable to save \";\"."), outputPath);// wxT("\";\"を保存できません。")
		return;
	}

	ofs << html;

	ShowInformation(_("The files have been merged."));// wxT("HTMLファイルを統合しました。")

	// 成功した場合はすぐにダイアログを閉じる
	Close();
}

void MergeHtmlDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
	Close();
}
