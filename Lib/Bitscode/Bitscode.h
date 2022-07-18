#pragma once

#include <string>
#include <cstring>
#include <vector>

void CharToString(std::string &dest_str,char *source_charArray);

void StringToChar(char *dest_charArray,const std::string &source_str);

std::vector<std::string> StringSplit(const std::string &str,const std::string &pattern);