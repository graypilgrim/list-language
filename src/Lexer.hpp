#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

enum LexerState {
	START,
	WORD,
	INT_NUMBER,
	FLOAT_NUMBER
};

class Lexer
{
public:
	Lexer();
	Lexer(const std::shared_ptr<std::istream> &f);

	void setStream(const std::shared_ptr<std::istream> &f);
	size_t getLineNo();
	std::string getNextAtom();
	std::string getCurrentLine();
	bool isStreamSet();

private:

 	bool inStartState(const char &sign, std::string &result);
	bool inWordState(const char &sign, std::string &result);
	bool inIntNumberState(const char &sign, std::string &result);
	bool inFloatNumberState(const char &sign, std::string &result);

	void skipComment();

	inline bool isLetter(const char &c);
	inline bool isDigit(const char &c);
	inline bool isOperator(const char &c);
	inline bool isColon(const char &c);
	inline bool isNewLine(const char &c);
	inline bool isWhitespace(const char &c);
	inline bool isCommentSignSkipMaybe(const char &c);
	inline bool isDot(const char &c);
	inline bool isLogicTie(const char &c);

	std::shared_ptr<std::istream> input;

	size_t currentLineNo;
	LexerState state;
};

#endif
