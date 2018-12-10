#include "SettingDialog.h"
#include "Command.h"
#include "log_file.h"

#include <wx/statline.h>
#include <wx/tokenzr.h> 

SettingDialog::SettingDialog(const wxString& title, wxFont& font, Setting& setting)
	: wxDialog(NULL, -1, title, wxDefaultPosition, wxSize(800, 640), wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX),
	  mSetting(setting)
{
	SetFont(font);

	auto vbox = new wxBoxSizer(wxVERTICAL);
	vbox->AddSpacer(14);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stLZ = new wxStaticText(this, wxID_ANY, _("Leela Zero:"));// wxT("Leela Zero:")
			hbox->Add(stLZ, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

			// フォーカスがどこにも設定されていない状態で
			// テキストコントロールにテキストを設定すると
			// 少なくとも日本語版のWindowsでは、
			// テキストが長い時にその表示位置がずれるので
			// 先にどこかへSetFocus()しておく必要がある。
			stLZ->SetFocus();
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				txLZ = new wxTextCtrl(this, wxID_ANY, mSetting.lz);
				hbox2->Add(txLZ, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

				// 英語にさほど詳しくないのでBrowseの後の...が何を意味するのかわからないが、
				// 英語表記のソフトには...がついていることが多いように思うのでつけておく。
				auto btLZ = new wxButton(this, CMD_CHOOSE_PATH_LZ, _("Browse..."), wxDefaultPosition, wxDefaultSize);// wxT("参照")
				hbox2->Add(btLZ, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 4, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	{
		auto hbox= new wxBoxSizer(wxHORIZONTAL);
		{
			auto stNetworkWeights = new wxStaticText(this, wxID_ANY, _("Network weights:"));// wxT("Network weights:")
			hbox->Add(stNetworkWeights, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				txNetworkWeights = new wxTextCtrl(this, wxID_ANY, mSetting.networkWeights);
				hbox2->Add(txNetworkWeights, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
				auto btNetworkWeights = new wxButton(this, CMD_CHOOSE_PATH_WEIGHTS, _("Browse..."), wxDefaultPosition, wxDefaultSize);// wxT("参照")
				hbox2->Add(btNetworkWeights, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 4, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stNumberOfThreads = new wxStaticText(this, wxID_ANY, _("Number of threads:"));// wxT("スレッド数:")
			hbox->Add(stNumberOfThreads, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				spNumberOfThreads = new wxSpinCtrl(this, wxID_ANY);
				spNumberOfThreads->SetRange(1, 64);
				spNumberOfThreads->SetValue(mSetting.numberOfThreads);
				hbox2->Add(spNumberOfThreads, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stNumberOfVisits = new wxStaticText(this, wxID_ANY, _("Number of visits:"));// wxT("探索数:")
			hbox->Add(stNumberOfVisits, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				spNumberOfVisits = new wxSpinCtrl(this, wxID_ANY);
				spNumberOfVisits->SetRange(30, 10000000);
				spNumberOfVisits->SetValue(mSetting.numberOfVisits);
				hbox2->Add(spNumberOfVisits, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stNumberOfProblems = new wxStaticText(this, wxID_ANY, _("Number of problems:"));// wxT("問題数:")
			hbox->Add(stNumberOfProblems, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				spNumberOfProblems = new wxSpinCtrl(this, wxID_ANY);
				spNumberOfProblems->SetRange(1, 500);
				spNumberOfProblems->SetValue(mSetting.numberOfProblems);
				hbox2->Add(spNumberOfProblems, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stRange = new wxStaticText(this, wxID_ANY, _("Search range:"));// wxT("範囲:")
			hbox->Add(stRange, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				spStart = new wxSpinCtrl(this, wxID_ANY);
				spStart->SetRange(1, 500);
				spStart->SetValue(mSetting.start);
				hbox2->Add(spStart, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
				auto stDash = new wxStaticText(this, wxID_ANY, wxT("-"));
				hbox2->Add(stDash, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
				spLast = new wxSpinCtrl(this, wxID_ANY);
				spLast->SetRange(1, 500);
				spLast->SetValue(mSetting.last);
				hbox2->Add(spLast, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stMaximumNumberOfMoves = new wxStaticText(this, wxID_ANY, _("Number of moves:"));// "最大の手数:"
			hbox->Add(stMaximumNumberOfMoves, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				spMaximumNumberOfMoves = new wxSpinCtrl(this, wxID_ANY);
				spMaximumNumberOfMoves->SetRange(2, 50);
				spMaximumNumberOfMoves->SetValue(mSetting.maximumNumberOfMoves);
				hbox2->Add(spMaximumNumberOfMoves, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stCharacterSet = new wxStaticText(this, wxID_ANY, _("Character set:"));// wxT("文字コード:")
			hbox->Add(stCharacterSet, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				chCharacterSet.Add(_("UTF-8"));// wxT("UTF-8")
				chCharacterSet.Add(_("Japanese (Shift_JIS) "));// wxT("日本語（Shift_JIS）")
				chCharacterSet.Add(_("Chinese (GB2312) "));// wxT("中国語（GB2312）")
				chCharacterSet.Add(_("Korean (EUC-KR) "));// wxT("韓国語（EUC-KR）")
				cbCharacterSet = new wxComboBox(this, wxID_ANY, wxEmptyString,
					wxDefaultPosition, wxDefaultSize, chCharacterSet);
				cbCharacterSet->SetSelection(static_cast<int>(setting.dafaultCharacterSet));
				cbCharacterSet->SetEditable(false);
				hbox2->Add(cbCharacterSet, 0, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 3, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		chPlayer.Add(_("Black"));// wxT("黒番")
		chPlayer.Add(_("White"));// wxT("白番")
		chPlayer.Add(_("Both"));// wxT("両方")
		chPlayer.Add(_("By name"));// wxT("名前")
		rbPlayer = new wxRadioBox(this, -1, _("Player"),// wxT("対局者")
			wxDefaultPosition, wxDefaultSize, chPlayer, 4, wxRA_HORIZONTAL);
		rbPlayer->SetSelection(static_cast<int>(mSetting.player));
		vbox->Add(rbPlayer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);
	{
		auto hbox = new wxBoxSizer(wxHORIZONTAL);
		{
			auto stPlayerNames = new wxStaticText(this, wxID_ANY, _("Player names:"));// wxT("対局者名:")
			hbox->Add(stPlayerNames, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
			auto hbox2 = new wxBoxSizer(wxHORIZONTAL);
			{
				wxString playerNames;
				bool firstTime = true;
				for (const auto& s : mSetting.playerNames) {
					if (!firstTime) {
						playerNames += wxT(";");
					}
					playerNames += s;
					firstTime = false;
				}
				txPlayerNames = new wxTextCtrl(this, wxID_ANY, playerNames);
				hbox2->Add(txPlayerNames, 1, wxALIGN_CENTER_VERTICAL);
			}
			hbox->Add(hbox2, 4, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
	}

	vbox->AddStretchSpacer(1);
	{
		auto btClose = new wxButton(this, CMD_CLOSE, _("Close"));// wxT("閉じる")
		btClose->SetFocus();
		vbox->Add(btClose, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 8);
	}
	vbox->AddSpacer(6);

	Bind(wxEVT_BUTTON, &SettingDialog::OnChoosePathLZ, this, CMD_CHOOSE_PATH_LZ);
	Bind(wxEVT_BUTTON, &SettingDialog::OnChoosePathWeights, this, CMD_CHOOSE_PATH_WEIGHTS);
	Bind(wxEVT_BUTTON, &SettingDialog::OnClose, this, CMD_CLOSE);
	Bind(wxEVT_CLOSE_WINDOW, &SettingDialog::OnCloseDialog, this);

	SetSizer(vbox);
	Center();
}

void SettingDialog::OnChoosePathLZ(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dialog(this, _("Choose a Leela Zero binary"),// wxT("リーラゼロの実行ファイルを選択")
		wxT(""), wxT(""), _("All files (*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);// wxT("すべてのファイル (*.*)|*.*")
	if (dialog.ShowModal() == wxID_OK)
	{
		txLZ->SetValue(dialog.GetPath());
	}
}

void SettingDialog::OnChoosePathWeights(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog dialog(this, _("Choose a network weights file"),// wxT("ネットワークウエイトファイルを選択")
		wxT(""), wxT(""), _("All files (*)|*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);// wxT("すべてのファイル (*.*)|*.*")
	if (dialog.ShowModal() == wxID_OK)
	{
		txNetworkWeights->SetValue(dialog.GetPath());
	}
}

void SettingDialog::OnClose(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void SettingDialog::OnCloseDialog(wxCloseEvent& event)
{
	mSetting.lz = txLZ->GetValue();
	mSetting.networkWeights = txNetworkWeights->GetValue();
	mSetting.numberOfThreads = spNumberOfThreads->GetValue();
	mSetting.numberOfVisits = spNumberOfVisits->GetValue();
	mSetting.numberOfProblems = spNumberOfProblems->GetValue();
	mSetting.start = spStart->GetValue();
	mSetting.last = spLast->GetValue();
	int m = spMaximumNumberOfMoves->GetValue();

	// 最大の手数は元の局面と変化図の手番を一致させるため偶数にする
	if (m % 2 == 1) {
		--m;
	}
	mSetting.maximumNumberOfMoves = m;
	mSetting.dafaultCharacterSet = static_cast<CharacterSet>(cbCharacterSet->GetSelection());
	mSetting.player = static_cast<Player>(rbPlayer->GetSelection());
	auto playerNames = txPlayerNames->GetValue();
	auto tokens = wxStringTokenize(playerNames, wxT(";"));
	size_t count = tokens.GetCount();
	mSetting.playerNames.clear();
	for (size_t n = 0U; n < count; ++n) {
		if (!tokens[n].empty()) {
			mSetting.playerNames.push_back(String(tokens[n]));
		}
	}

	mSetting.save();

	event.Skip();
}
