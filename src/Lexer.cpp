#include "Lexer.hpp"

Lexer::Lexer()
	: state(LexerState::START)
{}

Lexer::Lexer(const std::shared_ptr<std::istream> &stream)
	: input(stream), state(LexerState::START)
{}

void Lexer::setStream(const std::shared_ptr<std::istream> &stream) {
	input = stream;
	currentLineNo = 1;
}

size_t Lexer::getLineNo() {
	return currentLineNo;
}

std::string Lexer::getNextAtom() {
	std::string result;
	char sign;

	while (*input >> std::noskipws >> sign) {
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

	return result;
}

bool Lexer::isStreamSet() {
	return input.get() != nullptr;
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
		// ++index;
		return true;
	}

	if (isNewLine(sign) || isWhitespace(sign))
		return false;

	if (isCommentSignSkipMaybe(sign))
		return true;

	if (isLogicTie(sign)) {
		char nextSign;

		if (!(*input >> std::noskipws >> nextSign))
			return false;

		if (nextSign == sign) {
			// index += 2;
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
	if (isLetter(sign) || isDigit(sign)) {
		result += sign;
		return false;
	}

	if (isOperator(sign) || isColon(sign) || isWhitespace(sign)) {
		state = LexerState::START;
		input->unget();
		return true;
	}

	if (isCommentSignSkipMaybe(sign))
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
		input->unget();
		return true;
	}

	if (isWhitespace(sign) || isCommentSignSkipMaybe(sign)) {
		input->unget();
		return true;
	}


	if (isDot(sign)) {
		state = LexerState::FLOAT_NUMBER;
		result += sign;

		char nextSign;
		if (!(*input >> std::noskipws >> nextSign))
			return false;

		if (!isDigit(nextSign))
			throw std::domain_error("number expected after dot");

		state = LexerState::START;
		result += nextSign;

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
		input->unget();
		return true;
	}

	if (isWhitespace(sign) || isCommentSignSkipMaybe(sign)) {
		input->unget();
		return true;
	}


	if (isDot(sign)) {
		state = LexerState::START;
		throw std::domain_error("second dot not expected");
	}

	return false;
}

void Lexer::skipComment() {
	char sign;

	while (*input >> std::noskipws >> sign) {
		if (sign == '\n')
			return;
	}
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

bool Lexer::isNewLine(char c) {
	if (c == '\n') {
		++currentLineNo;
		return true;
	}

	return false;
}

bool Lexer::isWhitespace(char c) {
	return c <= ' ';
}

bool Lexer::isCommentSignSkipMaybe(char c) {
	if (c == '@') {
		skipComment();
		return c == '@';
	}

	return false;
}

bool Lexer::isDot(char c) {
	return c == '.';
}

bool Lexer::isLogicTie(char c) {
	return c == '&' || '|';
}
