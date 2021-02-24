#pragma once

#include <sstream>
#include <iostream>

#include "../mgs/script/gcx.h"
#include "../mgs/common/strcode.h"
#include "../mgs/common/game/game.h"
#include "../dictionary/dictionary.h"
#include "../indentation/indentation.h"

class Decompiler {
public:
	Decompiler(const GcxProc& proc, std::string procName);
	~Decompiler();
	void decompile();
	void decompileResource(int size);

private:

	int ptr = 0;
	GcxProc procBuffer;
	std::string procName;
	IndentationManager indentation;

	bool isIf = false;
	bool isInline = false;
	bool isFirstRun = true;

	int getSize();
	int getShortSize();

	void process();
	void processType();
	void processSize(uint32_t& size);
	void ProcessExpr(const uint8_t& op, std::string value1, std::string value2);
	void processTag(const uint8_t& tag);
	void processFor(const int& start, const int& size);

	uint8_t getTag();

	//print helpers
	template <class T>
	void readAndPrint(T& t, int size);
	void openBrace();
	void closeBrace();
	void printNewLine();
	void printProcSig();
	void printProcName();


	//read helpers
	void readLetter();
	void readCmdHeader();

	//expr helpers
	void calcExpr(std::vector<std::string>* exprStack);
	void captureValues(uint8_t tag, std::vector<std::string>* exprStack);

	//special funcs
	void mgs4param();
	void mgs3param();
	void processResource();
	void readIf(int start, int size, bool isParam = 0, bool isElse = 0);

	//tag funcs
	void readCmd();
	void readNum();
	void readVar();
	void readProc();
	void readEval();
	void readArgs();
	void readExpr();
	void readLocal();
	void readParam();

	//type funcs
	void readEnd();
	void readByte();
	void readLong();
	void readArray();
	void readShort();
	void readString();
	void readStrRes();
	void readUShort();
	uint32_t readStrCode(bool hasLetter = 0);

	//extra
	void readUByte();
	void readULong();
};
