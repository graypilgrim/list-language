#include <iostream>

#include "Lexer.hpp"

int main() {
	Lexer::instance().openFile("LICENSE");

	Lexer::instance().openFile("makefile1");
}
