#pragma once

#include "setting.h"

#include <wx/wx.h>
#include <wx/spinctrl.h>

class SettingDialog : public wxDialog
{
public:
	SettingDialog(const wxString& title, wxFont& font, Setting& setting);

	void OnChoosePathLZ(wxCommandEvent& event);
	void OnChoosePathWeights(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnCloseDialog(wxCloseEvent& event);

private:
	Setting& mSetting;

	wxTextCtrl* txLZ;
	wxTextCtrl* txNetworkWeights;
	wxSpinCtrl* spNumberOfThreads;
	wxSpinCtrl* spNumberOfVisits;
	wxSpinCtrl* spNumberOfProblems;
	wxSpinCtrl* spStart;
	wxSpinCtrl* spLast;
	wxSpinCtrl* spMaximumNumberOfMoves;
	wxComboBox* cbCharacterSet;
	wxArrayString chCharacterSet;
	wxRadioBox* rbPlayer;
	wxArrayString chPlayer;
	wxTextCtrl* txPlayerNames;
};
