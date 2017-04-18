#include "Lexer.hpp"

Lexer::Lexer()
:state(LexerState::START)
{}

bool Lexer::openFile(std::string fileName) {
	std::shared_ptr<std::ifstream> f(new std::ifstream);
	f.get()->open(fileName, std::ifstream::in);

	if (f.get()->bad())
		return false;


	input = f;
	resetIndicators();
	return true;
}

void Lexer::setStream(std::shared_ptr<std::istream> f) {
	input = f;
	resetIndicators();
}

size_t Lexer::getLineNo() {
	return lineNo;
}

std::string Lexer::getNextAtom() {
	std::string result;
	for (; index < currentLine.size(); ++index) {
		auto sign = currentLine[index];

		switch(state) {
		case START:
			if (isLetter(sign)) {
				state = LexerState::WORD;
				result += sign;
				break;
			}

			if (isDigit(sign)) {
				state = LexerState::INT_NUMBER;
				result += sign;
				break;
			}

			if (isOperator(sign) || isColon(sign)) {
				state = LexerState::START;
				result += sign;
				++index;
				return result;
			}

			if (isWhitespace(sign))
				break;

			if (isCommentSign(sign))
				return result;

			if (isLogicTie(sign)) {
				if ((index + 1) < currentLine.size() && (currentLine[index + 1] == sign)) {
					index += 2;
					result += sign;
					result += sign;
					return result;
				} else {
					throw std::domain_error("unrecognized operator");
				}
			}

			break;

		case WORD:
			if (isLetter(sign)) {
				result += sign;
				break;
			}

			if (isDigit(sign)) {
				result += sign;
				break;
			}

			if (isOperator(sign) || isColon(sign) || isWhitespace(sign)) {
				state = LexerState::START;
				return result;
			}

			if (isCommentSign(sign))
				return result;

			break;

		case INT_NUMBER:
			if (isLetter(sign)) {
				state = LexerState::START;
				throw std::domain_error("letter not expected");
			}

			if (isDigit(sign)) {
				result += sign;
				break;
			}

			if (isOperator(sign) || isColon(sign)) {
				state = LexerState::START;
				return result;
			}

			if (isWhitespace(sign))
				return result;

			if (isCommentSign(sign))
				return result;

			if (isDot(sign)) {
				state = LexerState::FLOAT_NUMBER;
				result += sign;
				if ((index + 1) < currentLine.size() && !isDigit(currentLine[index + 1])) {
					state = LexerState::START;
					throw std::domain_error("number expected after dot");
				}

				break;
			}

			break;

		case FLOAT_NUMBER:
			if (isLetter(sign)) {
				state = LexerState::START;
				throw std::domain_error("letter not expected");
			}

			if (isDigit(sign)) {
				result += sign;
				break;
			}

			if (isOperator(sign) || isColon(sign)) {
				state = LexerState::START;
				return result;
			}

			if (isWhitespace(sign))
				return result;

			if (isCommentSign(sign))
				return result;

			if (isDot(sign)) {
				state = LexerState::START;
				throw std::domain_error("second dot not expected");
			}

			break;
		}
	}

	if (index == currentLine.size())
		readNextLine();

	return result;
}

std::string Lexer::getCurrentLine() {
	return currentLine;
}

void Lexer::readNextLine() {
	if (!input.get()->eof()) {
		currentLine.clear();
		std::getline(*input.get(), currentLine);
		++lineNo;
	}
}

bool Lexer::isStreamSet() {
	return input.get() != nullptr;
}

void Lexer::resetIndicators() {
	lineNo = 0;
	index = 0;
	readNextLine();
}

void Lexer::startStateCondition(char c) {

}

bool Lexer::isLetter(char c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool Lexer::isDigit(char c) {
	return c >= '0' && c <= '9';
}

bool Lexer::isOperator(char c) {
	return c == '=' || c == '+' || c == '-' || c == '*' || c == '/';
}

bool Lexer::isColon(char c) {
	return c == ';';
}

bool Lexer::isWhitespace(char c) {
	return c <= ' ';
}

bool Lexer::isCommentSign(char c) {
	return c == '@';
}

bool Lexer::isDot(char c) {
	return c == '.';
}

bool Lexer::isLogicTie(char c) {
	return c == '&' || '|';
}
