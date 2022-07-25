#pragma once

#include <string>
#include <cstring>
#include <vector>

void CharToString(std::string &dest_str,char *source_charArray);

void StringToChar(char *dest_charArray,const std::string &source_str);

std::vector<std::string> StringSplit(const std::string unitedString, const std::string pattern);

std::string StringUnite(const std::vector<std::string> splitStringArray, const std::string pattern);