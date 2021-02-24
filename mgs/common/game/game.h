#pragma once
#include <map>
#include <regex>
#include <string>
#include <fstream>

enum _GAME {
	MGS3,
	MGS4
};

extern _GAME GAME;
extern std::map<unsigned int, std::string> commandMap;
extern void loadCommands(const std::string& dictionary);