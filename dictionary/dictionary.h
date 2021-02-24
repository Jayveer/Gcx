#pragma once

#include <map>
#include <string>
#include <fstream>

extern std::map<unsigned int, std::string> strcodeMap;
extern void loadDictionary(const std::string& dictionary);