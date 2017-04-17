#ifndef LEXER_HPP_
#define LEXER_HPP_

#include <iostream>
#include <string>
#include <fstream>

class Lexer
{
public:
	static Lexer& instance();
	void openFile(std::string fileName);
	void setFile(std::ifstream& f);
	size_t getLineNo();
	std::string getAtom();
	std::string getCurrentLine();

private:
	Lexer();
	~Lexer();
	void closeFile();

	std::ifstream file;

	size_t lineNo;
	std::string currentLine;
};

#endif
