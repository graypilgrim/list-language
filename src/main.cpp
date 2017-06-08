#include <iostream>

#include "Lexer.hpp"
#include "Parser.hpp"

int main() {
	auto lexer = std::make_shared<Lexer>(std::cin);
	auto parser = std::make_shared<Parser>(lexer);

	auto tree = parser->run();
	tree->execute();

}
