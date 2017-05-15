#include "Lexer.hpp"

Lexer::Lexer()
	: state(LexerState::START), ungetFlag(false)
{}

Lexer::Lexer(const std::shared_ptr<std::istream> &stream)
	: input(stream), state(LexerState::START), ungetFlag(false)
{}

void Lexer::setStream(const std::shared_ptr<std::istream> &stream) {
	input = stream;
	currentLineNo = 1;
}

size_t Lexer::getLineNo() {
	return currentLineNo;
}

std::string Lexer::getNextAtom() {
	if (ungetFlag) {
		ungetFlag = false;
		return cachedAtom;
	}

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

	cachedAtom = result;
	return result;
}

void Lexer::ungetAtom() {
	ungetFlag = true;
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

	if (isLogicOperator(sign)) {
		char nextSign;

		if (!(*input >> std::noskipws >> nextSign))
			return false;

		if ((sign == '<' || sign == '>' || sign == '=') && nextSign == '='){
			result += sign;
			result += nextSign;
			return true;
		} else if (sign == '<' || sign == '>') {
			input->unget();
			result += sign;
			return true;
		} else if (sign == '=') {
			input->unget();
		}
	}

	if (isOperator(sign) || isSemicolon(sign) || isBracket(sign) || isComa(sign)) {
		result += sign;
		return true;
	}

	return false;
}

bool Lexer::inWordState(const char &sign, std::string &result) {
	if (isLetter(sign) || isDigit(sign)) {
		result += sign;
		return false;
	}

	if (isOperator(sign) || isSemicolon(sign) || isWhitespace(sign) || isBracket(sign) || isComa(sign)) {
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

	if (isOperator(sign) || isSemicolon(sign) || isComa(sign)) {
		state = LexerState::START;
		input->unget();
		return true;
	}

	if (isWhitespace(sign) || isCommentSignSkipMaybe(sign)) {
		state = LexerState::START;
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

	if (isOperator(sign) || isSemicolon(sign) || isComa(sign)) {
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

bool Lexer::isLetter(const char &c) {
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool Lexer::isDigit(const char &c) {
	return c >= '0' && c <= '9';
}

bool Lexer::isOperator(const char &c) {
	return c == '=' || c == '+' || c == '-' || c == '*' || c == '/';
}

bool Lexer::isSemicolon(const char &c) {
	return c == ';';
}

bool Lexer::isNewLine(const char &c) {
	if (c == '\n') {
		++currentLineNo;
		return true;
	}

	return false;
}

bool Lexer::isWhitespace(const char &c) {
	return c <= ' ';
}

bool Lexer::isCommentSignSkipMaybe(const char &c) {
	if (c == '@') {
		skipComment();
		return c == '@';
	}

	return false;
}

bool Lexer::isDot(const char &c) {
	return c == '.';
}

bool Lexer::isLogicTie(const char &c) {
	return c == '&' || c == '|';
}

bool Lexer::isLogicOperator(const char &c) {
	return c == '<' || c == '>' || c == '=';
}

bool Lexer::isBracket(const char &c) {
	return c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']';
}

bool Lexer::isComa(const char &c) {
	return c == ',';
}
