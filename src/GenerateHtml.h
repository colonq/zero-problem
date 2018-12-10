#pragma once

#include "MainWindow.h"
#include "analysis.h"

void GenerateHtml(MainWindow* window, const String& path,
	const std::string& title, const std::string& id);
