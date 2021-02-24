#include "game.h"

std::map<unsigned int, std::string> commandMap;

_GAME GAME = MGS4;

void loadCommands(const std::string& commands) {
	std::string str;
	std::ifstream file(commands);

	if (!file) return;

	while (std::getline(file, str)) {
		const char* regex = "([^\\s]+)(\\s+)?->(\\s+)?([^\\s]+)";
		std::regex re(regex);
		std::smatch match;

		if (std::regex_search(str, match, re)) {
			int n = stoi(match[1], 0, 16);
			commandMap[n] = match[4];
		}
	}
}