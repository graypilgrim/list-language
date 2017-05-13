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
	void readNextLine();
	void resetIndicators();

 	bool inStartState(const char &sign, std::string &result);
	bool inWordState(const char &sign, std::string &result);
	bool inIntNumberState(const char &sign, std::string &result);
	bool inFloatNumberState(const char &sign, std::string &result);

	inline bool isLetter(char c);
	inline bool isDigit(char c);
	inline bool isOperator(char c);
	inline bool isColon(char c);
	inline bool isWhitespace(char c);
	inline bool isCommentSign(char c);
	inline bool isDot(char c);
	inline bool isLogicTie(char c);

	std::shared_ptr<std::istream> input;

	size_t lineNo;
	size_t index;
	LexerState state;
	std::string currentLine;
};

#endif
