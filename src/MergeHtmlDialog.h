#pragma once

#include <wx/wx.h>

class MergeHtmlDialog : public wxDialog
{
public:
	MergeHtmlDialog(const wxString& title, const wxFont& font);

private:
	void MakeListBox(wxBoxSizer* vbox);
	void MakeButtons(wxBoxSizer* vbox);

	void UpdateButtonState();

	void ShowInformation(const wxString& message) const;
	void ShowErrorMessage(const wxString& formatString, const wxString& s) const;
	void ShowErrorMessage(const wxString& message) const;

	void AppendFiles(const wxArrayString& paths);
	void OnAppend(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnClear(wxCommandEvent& event);
	void OnMerge(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);

	wxArrayString asFileList;
	wxListBox* lbFileList;
	wxButton* btDelete;
	wxButton* btClear;
	wxButton* btMerge;

	friend class FileDropTargetForMergeHtmlDialog;
};
