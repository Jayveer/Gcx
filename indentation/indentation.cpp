#include "indentation.h"
#include <iostream>

IndentationManager::IndentationManager(int indentation) {
	m_indentation = indentation;
}

IndentationManager::IndentationManager() {
}

IndentationManager::~IndentationManager() {
}


void IndentationManager::indent() {
	m_indentation++;
}

void IndentationManager::printIndent() {
	for (int i = 0; i < m_indentation; i++)
		std::cout << "\t";
}

void IndentationManager::unindent() {
	if (m_indentation > 0) m_indentation--;
}

void IndentationManager::setIndentation(int indentation) {
	m_indentation = indentation;
}

void IndentationManager::resetIndentation() {
	setIndentation(0);
}