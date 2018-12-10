#include "time_string.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <wx/wx.h>

TimeString GetTimeString()
{
	TimeString timeString;
	const auto now = std::time(nullptr);
	{
		std::ostringstream o;
		o.imbue(std::locale(""));
		const auto s = wxString(_("Go_%Y_%m_%d_%H_%M_%S")) + wxT(".html");// wxT("囲碁_%Y_%m_%d_%H_%M_%S")
		const std::string fmt = s.ToUTF8().data();
		o << std::put_time(std::localtime(&now), fmt.c_str());
		timeString.fileName = ToString(o.str());
	}
	{
		std::ostringstream o;
		o.imbue(std::locale(""));
		const auto s = wxString(_("Go_%Y_%m_%d_%H_%M_%S")) + wxT("_M.html");// wxT("囲碁_%Y_%m_%d_%H_%M_%S")
		const std::string fmt = s.ToUTF8().data();
		o << std::put_time(std::localtime(&now), fmt.c_str());
		timeString.fileName_M = ToString(o.str());
	}
	{
		std::ostringstream o;
		o.imbue(std::locale(""));
		const auto s = wxString(_("%c"));
		const std::string fmt = s.ToUTF8().data();
		o << std::put_time(std::localtime(&now), fmt.c_str());
		timeString.title = o.str();
	}
	{
		std::ostringstream o;
		o.imbue(std::locale(""));
		const auto s = wxString(_("%c")) + wxT(" (M)");
		const std::string fmt = s.ToUTF8().data();
		o << std::put_time(std::localtime(&now), fmt.c_str());
		timeString.title_M = o.str();
	}
	{
		std::ostringstream o;
		o << std::put_time(std::localtime(&now), "%Y%m%d%H%M%S");
		timeString.id = o.str();
	}
	return timeString;
}
