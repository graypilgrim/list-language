#include "Lexer.hpp"

bool Lexer::openFile(std::string fileName) {
	std::shared_ptr<std::ifstream> f(new std::ifstream);
	f.get()->open(fileName, std::ifstream::in);

	if (f.get()->bad())
		return false;


	input = f;
	lineNo = 0;
	readNextLine();
	return true;
}

void Lexer::setStream(std::shared_ptr<std::istream> f) {
	input = f;
	lineNo = 0;
	readNextLine();
}

size_t Lexer::getLineNo() {
	return lineNo;
}

std::string Lexer::getAtom() {

}

std::string Lexer::getCurrentLine() {
	return currentLine;
}

void Lexer::readNextLine() {
	if (!input.get()->eof()) {
		std::getline(*input.get(), currentLine);
		++lineNo;
	}
}

bool Lexer::isStreamSet() {
	return input.get() != nullptr;
}
