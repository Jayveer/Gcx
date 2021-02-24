#pragma once

class IndentationManager {
public:
	int m_indentation = 0;
	IndentationManager(int indentation);
	IndentationManager();
	~IndentationManager();
	void indent();
	void unindent();
	void printIndent();
	void resetIndentation();
	void setIndentation(int indentation);
};