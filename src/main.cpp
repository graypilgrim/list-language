#include <iostream>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"

int main() {
	auto lexer = std::make_shared<Lexer>(std::cin);
	auto parser = std::make_shared<Parser>(lexer);

	auto interpreter = std::make_shared<Interpreter>(parser->run());
	interpreter->execute();
}
