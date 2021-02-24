#pragma once
#include "../../mgs/common/fileutil.h"
#include "../../decompiler/decompiler.h"

class CLI {
public:
	CLI(int argc, char** argv);
	~CLI();

	void run(std::string programName, std::string version);
	void exit();

	enum COMMAND {
		DECOMPILE,
		COMPILE
	};

private:
	int argc;
	char** argv;
	int currentArg = 1;
	CLI::COMMAND command;
	bool exportRes = false;

	void printUsage();
	bool checkInput();
	void processArgs();
	void processFile();
	void compileFile();
	void decompileFile();
	void processCommands();
	void initDictionaries();
	bool isCommand(char* arg);
	void setCommand(char* arg);
	void decompileProcs(Gcx& gcx, const std::string& input, std::string& output);
	void exportResources(Gcx& gcx, const std::string& input, std::string& output);

	const char* EXIT_MESSAGE = "Exiting\n";
	const char* USAGE_MESSAGE = "Usage:\t gcx.exe [-OPT] <*.gcx> \n"
		"To decompile using the MGS3 version of Gcx use the -mgs3 option\n"
		"If you wish to export resources as well use the -res option\n";
};