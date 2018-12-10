#pragma once

#include "color.h"

#include <string>
#include <vector>

std::string GetHtml(const std::string& title, const std::vector<std::string>& problems, int boardSize);
std::string GetHtmlTitle(const std::string& html);
std::vector<std::string> GetProblems(const std::string& html);
Color GetProblemColor(const std::string& problem);
