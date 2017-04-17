#include "Lexer.hpp"

Lexer::Lexer()
:lineNo(1) {
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
}

Lexer::~Lexer() {
	closeFile();
}

Lexer& Lexer::instance() {
	static Lexer lexer;
	return lexer;
}

void Lexer::openFile(std::string fileName) {
	closeFile();

	try {
		file.open(fileName);
		std::getline(file, currentLine);
	}
	catch (const std::exception& e) {
		std::cout << "No file named: " << fileName << std::endl;
	}
}

void Lexer::setFile(std::ifstream& f) {
	file = std::move(f);
	lineNo = 1;
}

size_t Lexer::getLineNo() {
	return lineNo;
}

std::string Lexer::getAtom() {

}

std::string Lexer::getCurrentLine() {
	return currentLine;
}

void Lexer::closeFile() {
	if (file.is_open()) {
		try {
			file.close();
		}
		catch (...) {
		}
	}
}
