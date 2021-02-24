#include "decompiler.h"

Decompiler::Decompiler(const GcxProc& proc, std::string procName) {
	this->procBuffer = proc;
	this->procName = procName;
}

Decompiler::~Decompiler() {
}

//print util
template <class T>
void Decompiler::readAndPrint(T& t, int size) {
	t = *(T*)&procBuffer[ptr];
	ptr += size;
	std::cout << std::dec << +t;
}

void Decompiler::printNewLine() {
	if (!isInline)
		std::cout << "\n";
}

void Decompiler::printProcSig() {
	if (isFirstRun) {
		printProcName();
		isFirstRun = false;
	}
}

void Decompiler::printProcName() {
	std::cout << procName << " ";
}

void Decompiler::openBrace() {
	std::cout << "{";
	printNewLine();
	if (!isInline) { indentation.indent(); }
}

void Decompiler::closeBrace() {
	if (!isInline) { indentation.unindent(); }
	if (!isInline) { indentation.printIndent(); }
	std::cout << "}";
}

//read helpers
void Decompiler::readLetter() {
	uint8_t l = procBuffer[ptr];
	ptr++;
	std::cout << l;
}

void Decompiler::readCmdHeader() {
	int size = getShortSize();
	int start = ptr;

	processFor(start, size);
	std::cout << " \\";
	printNewLine();
}

//read expr
void Decompiler::captureValues(uint8_t tag, std::vector<std::string>* exprStack) {
	std::streambuf* old = std::cout.rdbuf();
	std::stringstream value;
	std::cout.rdbuf(value.rdbuf());
	process();
	std::cout.rdbuf(old);
	exprStack->push_back(value.str());
}

void Decompiler::calcExpr(std::vector<std::string>* exprStack) {
	uint8_t op = procBuffer[ptr];
	ptr++;

	op &= 0x1F;

	if (op == 0)
		return;

	std::string val1 = exprStack->back();
	exprStack->pop_back();

	std::string val2 = exprStack->back();
	exprStack->pop_back();

	std::streambuf* old = std::cout.rdbuf();
	std::stringstream value;
	std::cout.rdbuf(value.rdbuf());

	if (op == 0x16) { indentation.printIndent(); }
	std::cout << "(";  ProcessExpr(op, val1, val2); std::cout << ")";
	if (op == 0x16) { std::cout << "\n"; }
	std::cout.rdbuf(old);
	exprStack->push_back(value.str());
}


//read types
void Decompiler::readUShort() {
	uint16_t val;
	readAndPrint<uint16_t>(val, 2);
}

void Decompiler::readByte() {
	int8_t val;
	readAndPrint<int8_t>(val, 1);
}

void Decompiler::readUByte() {
	uint8_t val;
	readAndPrint<uint8_t>(val, 1);
}

uint32_t Decompiler::readStrCode(bool hasLetter) {
	uint8_t l;

	if (hasLetter) {
		l = procBuffer[ptr];
		ptr++;
	}

	uint32_t strcode = *(uint32_t*)&procBuffer[ptr];
	ptr += 3;
	strcode &= 0x00FFFFFF;

	if (commandMap[strcode] != "") {
		std::cout << std::hex << commandMap[strcode];
		return strcode;
	}


	if (strcodeMap[strcode] != "") {
		std::cout << strcodeMap[strcode];
	}
	else {
		if (hasLetter) std::cout << l;
		std::cout << std::hex << "[" << strcode << "]";
	}

	return strcode;
}

void Decompiler::readString() {
	uint8_t length = procBuffer[ptr];
	ptr++;
	std::string str;
	str.resize(length);
	memcpy(&str[0], &procBuffer[ptr], length);
	ptr += length;
	str.pop_back();
	//SetConsoleOutputCP(65001); 20932
	std::cout << "\"" << str << "\"";
}

void Decompiler::readArray() {
	readStrCode(0);
	uint8_t ap = procBuffer[ptr];
	ptr++;
	std::cout << std::dec << "[" << +ap << "]";
}

void Decompiler::readShort() {
	int16_t val;
	readAndPrint<int16_t>(val, 2);
}

void Decompiler::readLong() {
	int32_t val;
	readAndPrint<int32_t>(val, 4);
}

void Decompiler::readULong() {
	uint32_t val;
	readAndPrint<uint32_t>(val, 4);
}

void Decompiler::readStrRes() {
	//TODO: if > 8000 get res a diff way
	uint16_t val;
	std::cout << "$strres:";
	readAndPrint<uint16_t>(val, 2);
}

void Decompiler::readEnd() {

}

//read special commands
void Decompiler::readIf(int start, int size, bool isParam, bool isElse) {
	if (!isParam) {
		size = getShortSize();
		start = ptr;
	}

	if (!isElse) {
		std::cout << " ";
		readExpr();
	}

	std::cout << " ";
	openBrace();
	processFor(start, size);
	closeBrace();
}

//read tags

//0xC0
void Decompiler::readNum() {
	uint8_t val = procBuffer[ptr];
	ptr++;
	int output = (val & 0x3F) - 1;

	std::cout << std::to_string(output);
}

//0x90
void Decompiler::readLocal() {
	uint8_t lclArg = procBuffer[ptr];
	ptr++;
	
	lclArg &= 0x0F;
	std::cout << "$lclArg" << std::dec << +lclArg;
}

//0x80
void Decompiler::readProc() {
	int size = getSize();
	int start = ptr;

	if (!isInline) { indentation.printIndent(); }
	std::cout << "proc "; printProcSig();
	openBrace();

	processFor(start, size);

	closeBrace();
	printNewLine();
}

//0x70
void Decompiler::readEval() {
	int size = getSize();
	int start = ptr;

	if (!isInline) { indentation.printIndent(); }
	std::cout << "@proc";
	readShort();

	processFor(start, size);
	printNewLine();
}

//0x60
void Decompiler::readCmd() {
	int size = getSize();
	int start = ptr;

	if (!isInline) { indentation.printIndent(); }
	uint32_t strcode = readStrCode();
	if (strcode == 0xD86) {
		readIf(start, size);
	}
	else {
		readCmdHeader();
		indentation.indent();
		processFor(start, size);
		indentation.unindent();
	}

	printNewLine();
}

//0x50
void Decompiler::mgs3param() {
	int size = getSize();
	int start = ptr;

	readLetter();
	processFor(start, size);
}

void Decompiler::mgs4param() {
	int size = getSize();
	int start = ptr;

	uint32_t strcode = readStrCode(1);
	if (strcode == 0x69 && (getTag() == 0x30)) { //-i isn't always if
		readIf(start, size, 1);
	}
	else if (strcode == 0x65) {
		readIf(start, size, 1, 1);
	}
	else {
		processFor(start, size);
	}
}

void Decompiler::readParam() {
	if (!isInline) { indentation.printIndent(); }
	std::cout << "-";

	GAME == MGS3 ? mgs3param() : mgs4param();

	if (procBuffer[ptr] != 0x00) std::cout << " \\";
	printNewLine();
}

//0x40
void Decompiler::readArgs() {
	uint8_t argno = procBuffer[ptr];
	ptr++;
	
	argno &= 0x0F;

	if (argno == 0x0F) {
		uint8_t next = procBuffer[ptr];
		ptr++;
		argno += next;
	}

	std::cout << "$arg" << std::dec << +argno;
}

//0x30
void Decompiler::readExpr() {
	int size = getSize();
	int start = ptr;
	int length = start + size;
	std::vector<std::string>exprStack;

	isInline = true;
	while (ptr < length) {
		uint8_t type = procBuffer[ptr];
		((type & 0xE0) != 0xA0) ? captureValues(type, &exprStack) : calcExpr(&exprStack);
	}
	isInline = false;
	std::cout << exprStack[0];
}

//0x20
void Decompiler::readVar() {
	std::string bufp = "varbuf";
	uint32_t varcode = *(uint32_t*)&procBuffer[ptr];
	ptr += 4;

	varcode = _byteswap_ulong(varcode);

	uint8_t tag = (varcode & 0xF0000000) >> 24;
	uint32_t region = (varcode & 0xF00000);
	if (region == 0x800000) { bufp = "linkvarbuf"; }
	if (region == 0x100000) { bufp = "localvarbuf"; }

	uint32_t offset = varcode & 0xFFFF;

	std::cout << "$var:" << bufp << "[" << std::hex << offset << "]";

	if (tag == 0x20) {
		std::cout << "["; process(); std::cout << "]";
		std::cout << "["; process(); std::cout << "]";
	}
}

void Decompiler::ProcessExpr(const uint8_t& op, std::string value1, std::string value2) {
	switch (op) {
	case 0x00:												break;
	case 0x01: std::cout << "-" << value1;					break;
	case 0x02: std::cout << "!" << value1;					break;
	case 0x03: std::cout << "~" << value1;					break;
	case 0x04: std::cout << value2 << " + " << value1;		break;
	case 0x05: std::cout << value2 << " - " << value1;		break;
	case 0x06: std::cout << value2 << " * " << value1;		break;
	case 0x07: std::cout << value2 << " / " << value1;		break;
	case 0x08: std::cout << value2 << " % " << value1;		break;
	case 0x09: std::cout << value2 << " << " << value1;		break;
	case 0x0A: std::cout << value2 << " >> " << value1;		break;
	case 0x0B: std::cout << value2 << " == " << value1;		break;
	case 0x0C: std::cout << value2 << " != " << value1;		break;
	case 0x0D: std::cout << value2 << " < " << value1;		break;
	case 0x0E: std::cout << value2 << " <= " << value1;		break;
	case 0x0F: std::cout << value2 << " > " << value1;		break;
	case 0x10: std::cout << value2 << " >= " << value1;		break;
	case 0x11: std::cout << value2 << " | " << value1;		break;
	case 0x12: std::cout << value2 << " & " << value1;		break;
	case 0x13: std::cout << value2 << " ^ " << value1;		break;
	case 0x14: std::cout << value2 << " || " << value1;		break;
	case 0x15: std::cout << value2 << " && " << value1;		break;
	case 0x16: std::cout << value2 << " = " << value1;		break;
	}
}

void Decompiler::processType() {
	uint8_t type = procBuffer[ptr];
	ptr++;

	switch (type) {
	case 0x01: readShort();		break;
	case 0x02: readUByte();		break;
	case 0x03: readUByte();		break;
	case 0x04: readUByte();		break;
	case 0x06: readStrCode();	break;
	case 0x07: readString();	break;
	case 0x08: readUShort();	break;
	case 0x09: readLong();		break;
	case 0x0A: readULong();		break;
	case 0x0D: readArray();		break;
	case 0x0E: readStrRes();	break;
	case 0x00: readEnd();		break;
	}
}

void Decompiler::processTag(const uint8_t& tag) {
	switch (tag) {
	case 0xF0:
	case 0xE0:
	case 0xD0:
	case 0xC0: readNum();	break;
	case 0x90: readLocal();	break;
	case 0x80: readProc();	break;
	case 0x70: readEval();	break;
	case 0x60: readCmd();	break;
	case 0x50: readParam();	break;
	case 0x40: readArgs();	break;
	case 0x30: readExpr();	break;
	case 0x20: readVar();	break;
	case 0x10: readVar();	break;
	default:				break;
	}
}

void Decompiler::processSize(uint32_t& size) {
	switch (size) {
	case 0x0D: size = procBuffer[ptr];              ptr++;    break;
	case 0x0E: size = *(uint16_t*)&procBuffer[ptr]; ptr += 2; break;
	default: break;
	}
}

void Decompiler::processFor(const int& start, const int& size) {
	int length = start + size;

	while (ptr < length) {
		std::cout << " ";
		process();
	}
}

int Decompiler::getShortSize() {
	uint16_t size = procBuffer[ptr];
	ptr++;
	size &= 0xFF;

	if (size > 0x7F) {
		uint16_t size2 = procBuffer[ptr];
		ptr++;
		size2 &= 0xFF;

		size = (size << 8) | (size2);
		size &= 0x7FFF;
	}

	return size;
}

int Decompiler::getSize() {
	uint32_t size = procBuffer[ptr];
	ptr++;
	size &= 0x0F;
	processSize(size);
	return size;
}

uint8_t Decompiler::getTag() {
	return (procBuffer[ptr] & 0xF0);
}

void Decompiler::process() {
	uint8_t tag = getTag();
	tag ? processTag(tag) : processType();
}

void Decompiler::decompile() {
	process();
	printNewLine();
}


void Decompiler::processResource() {
	while (procBuffer[ptr]) {
		std::cout << " ";
		process();
	}
}

void Decompiler::decompileResource(int size) {
	std::cout << "proc "; printProcSig();
	openBrace();
	processResource();
	printNewLine();
	closeBrace();
	printNewLine();
	printNewLine();
}