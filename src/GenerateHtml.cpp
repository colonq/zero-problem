#include "GenerateHtml.h"
#include "Command.h"

#include <fstream>

void GenerateHtml(MainWindow* window, const String& path,
	const std::string& title, const std::string& id)
{
	Analysis analysis;
	ExitStatus exitStatus;

	const auto html = analysis.generateHtml(
		window->mSetting, window->mJobList,
		window->mSearchSpeed, window->shouldStopGenerating,
		title, id, exitStatus);
	window->shouldStopGenerating.store(false);

	// 終了状態ごとに処理を振り分ける
	if (exitStatus == ExitStatus::FAILED) {

		// 初期化に失敗した
		wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_GENERATION_FAILED));
	} else if (exitStatus == ExitStatus::CANCELED) {
		if (!html.empty()) {
			std::ofstream ofs(path);
			if (ofs) {
				ofs << html;
				ofs.close();

				// 問題の作成の中止と問題の保存に成功した
				wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_GENERATION_CANCELLED));
			} else {

				// 問題の作成の中止に成功したが問題の保存に失敗した
				wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_SAVE_ERROR));
			}
		} else {

			// 問題の作成の中止に成功した
			wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_GENERATION_CANCELLED));
		}
	} else {
		if (!html.empty()) {
			std::ofstream ofs(path);
			if (ofs) {
				ofs << html;
				ofs.close();

				// 問題の作成と保存に成功した
				wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_GENERATION_COMPLETED));
			} else {

				// 問題の作成に成功したがその保存に失敗した
				wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_SAVE_ERROR));
			}
		} else {

			// 問題の作成に失敗した
			wxQueueEvent(window, new wxThreadEvent(wxEVT_THREAD, CMD_GENERATION_FAILED));
		}
	}
}
