#include "cli.h"

CLI::CLI(int argc, char** argv) {
	this->argc = argc;
	this->argv = argv;
}

CLI::~CLI() {

}

void CLI::processCommands() {
	while (currentArg < 4 && isCommand(argv[currentArg])) {
		setCommand(argv[currentArg]);
		currentArg++;
	}
}

void CLI::setCommand(char* arg) {

	if (!strcmp(arg, "-MGS4") || !strcmp(arg, "-mgs4") || !strcmp(arg, "-4")) {
		GAME = MGS4;
		return;
	}

	if (!strcmp(arg, "-MGS3") || !strcmp(arg, "-mgs3") || !strcmp(arg, "-3")) {
		GAME = MGS3;
		return;
	}

	if (!strcmp(arg, "-COMPILE") || !strcmp(arg, "-compile") || !strcmp(arg, "-c") || !strcmp(arg, "-C")) {
		command = CLI::COMPILE;
		return;
	}

	if (!strcmp(arg, "-DECOMPILE") || !strcmp(arg, "-decompile") || !strcmp(arg, "-d") || !strcmp(arg, "-D")) {
		command = CLI::DECOMPILE;
		return;
	}

	if (!strcmp(arg, "-res") || !strcmp(arg, "-RES")) {
		exportRes = true;
		return;
	}

	if (!strcmp(arg, "-nores") || !strcmp(arg, "-NORES")) {
		exportRes = false;
		return;
	}

	printf("command not recognised\n");
}

void CLI::processArgs() {
	processCommands();
	processFile();
}

std::string makeOutputFilename(const std::string& input) {
	std::string extension = getExtension(input);
	std::string filename = "output.gcl";

	if (extension == ".gcx") {
		filename = getExtensionlessName(input) + ".gcl";
	}
	return filename;
}

void CLI::compileFile() {
	printf("compile is not currently supported\n");
	exit();
}

void CLI::decompileProcs(Gcx& gcx, const std::string& input, std::string& output) {
	std::streambuf* cout_buff = std::cout.rdbuf();

	std::stringstream decompiled;
	std::cout.rdbuf(decompiled.rdbuf());

	Decompiler main(gcx.getMainProc(), "main");
	main.decompile();

	for (int i = 0; i < gcx.getNumProc(); i++) {
		std::string name = "proc" + std::to_string(i + 1);
		Decompiler decompiler(gcx.getProc(i), name);
		decompiler.decompile();
	}

	std::cout.rdbuf(cout_buff);

	writeTextToFile(decompiled, makeOutputFilename(input), output);
}

void CLI::exportResources(Gcx& gcx, const std::string& input, std::string& output) {
	std::string foldername = getExtensionlessName(input) + "_strres";
	updateDir(foldername, output);

	for (int i = 0; i < gcx.getNumResource(); i++) {
		std::string filename = std::to_string(i) + ".bin";
		writeDataToFile(gcx.getResource(i), gcx.getResourceSize(i), filename, output);
	}

	resetDir(output);
}

void CLI::decompileFile() {
	std::string input = argv[currentArg];
	std::string output = "";
	initDictionaries();
	currentArg++;

	if (currentArg == argc - 1) output = argv[currentArg];

	Gcx gcx = Gcx(input);
	gcx.open();

	if (exportRes) exportResources(gcx, input, output);
	decompileProcs(gcx, input, output);
}

void CLI::processFile() {
	switch (command) {
	case CLI::DECOMPILE:
		decompileFile();
		break;
	case CLI::COMPILE:
		compileFile();
		break;
	default:
		decompileFile();
	}
}

bool CLI::checkInput() {
	if (argc > 1 && argc < 6) return true;
	printUsage();
	return false;
}

void CLI::initDictionaries() {
	loadDictionary("dictionary.txt");
	loadCommands("commands.txt");
}

void CLI::run(std::string programName, std::string version) {
	printf("Running %s v%s: Visit https://github.com/Jayveer/Gcx for updates:\n", programName.c_str(), version.c_str());
	if (!checkInput()) return;
	processArgs();
}

bool CLI::isCommand(char* arg) {
	return arg[0] == 0x2D;
}

void CLI::printUsage() {
	printf(this->USAGE_MESSAGE);
}

void CLI::exit() {
	printf(this->EXIT_MESSAGE);
}