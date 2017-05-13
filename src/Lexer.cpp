#include "Lexer.hpp"

Lexer::Lexer()
	: state(LexerState::START)
{}

Lexer::Lexer(const std::shared_ptr<std::istream> &stream)
	: input(stream), state(LexerState::START)
{}

void Lexer::setStream(const std::shared_ptr<std::istream> &stream) {
	input = stream;
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
			try {
				if (inStartState(sign, result))
					return result;
			} catch (std::domain_error &e) {
				throw e;
			}

			break;

		case WORD:
			try {
				if (inWordState(sign, result))
					return result;
			} catch (std::domain_error &e) {
				throw e;
			}

			break;

		case INT_NUMBER:
			try {
				if (inIntNumberState(sign, result))
					return result;
			} catch (std::domain_error &e) {
				throw e;
			}

			break;

		case FLOAT_NUMBER:
			try {
				if (inFloatNumberState(sign, result))
					return result;
			} catch (std::domain_error &e) {
				throw e;
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
		index = 0;
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

bool Lexer::inStartState(const char &sign, std::string &result) {
	if (isLetter(sign)) {
		state = LexerState::WORD;
		result += sign;
		return false;
	}

	if (isDigit(sign)) {
		state = LexerState::INT_NUMBER;
		result += sign;
		return false;
	}

	if (isOperator(sign) || isColon(sign)) {
		state = LexerState::START;
		result += sign;
		++index;
		return true;
	}

	if (isWhitespace(sign))
		return false;

	if (isCommentSign(sign))
		return true;

	if (isLogicTie(sign)) {
		if ((index + 1) < currentLine.size() && (currentLine[index + 1] == sign)) {
			index += 2;
			result += sign;
			result += sign;
			return true;
		} else {
			throw std::domain_error("unrecognized operator");
		}
	}

	return false;
}

bool Lexer::inWordState(const char &sign, std::string &result) {
	if (isLetter(sign)) {
		result += sign;
		return false;
	}

	if (isDigit(sign)) {
		result += sign;
		return false;
	}

	if (isOperator(sign) || isColon(sign) || isWhitespace(sign)) {
		state = LexerState::START;
		return true;
	}

	if (isCommentSign(sign))
		return true;

	return false;
}

bool Lexer::inIntNumberState(const char &sign, std::string &result) {
	if (isLetter(sign)) {
		state = LexerState::START;
		throw std::domain_error("letter not expected");
	}

	if (isDigit(sign)) {
		result += sign;
		return false;
	}

	if (isOperator(sign) || isColon(sign)) {
		state = LexerState::START;
		return true;
	}

	if (isWhitespace(sign))
		return true;

	if (isCommentSign(sign))
		return true;

	if (isDot(sign)) {
		state = LexerState::FLOAT_NUMBER;
		result += sign;
		if ((index + 1) < currentLine.size() && !isDigit(currentLine[index + 1])) {
			state = LexerState::START;
			throw std::domain_error("number expected after dot");
		}

		return false;
	}

	return false;
}

bool Lexer::inFloatNumberState(const char &sign, std::string &result) {
	if (isLetter(sign)) {
		state = LexerState::START;
		throw std::domain_error("letter not expected");
	}

	if (isDigit(sign)) {
		result += sign;
		return false;
	}

	if (isOperator(sign) || isColon(sign)) {
		state = LexerState::START;
		return true;
	}

	if (isWhitespace(sign))
		return true;

	if (isCommentSign(sign))
		return true;

	if (isDot(sign)) {
		state = LexerState::START;
		throw std::domain_error("second dot not expected");
	}

	return false;
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
