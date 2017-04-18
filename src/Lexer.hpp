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
	bool openFile(std::string fileName);
	void setStream(std::shared_ptr<std::istream> f);
	size_t getLineNo();
	std::string getNextAtom();
	std::string getCurrentLine();
	bool isStreamSet();

private:
	void readNextLine();
	void resetIndicators();

	void startStateCondition(char c);

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
