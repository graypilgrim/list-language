#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

class Lexer
{
public:
	bool openFile(std::string fileName);
	void setStream(std::shared_ptr<std::istream> f);
	size_t getLineNo();
	std::string getAtom();
	std::string getCurrentLine();
	bool isStreamSet();

private:
	void readNextLine();

	std::shared_ptr<std::istream> input;

	size_t lineNo;
	std::string currentLine;
};

#endif
