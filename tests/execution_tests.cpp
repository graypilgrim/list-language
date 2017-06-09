#include "lib/catch.hpp"
#include "../src/Parser.hpp"
#include "../src/Interpreter.hpp"

TEST_CASE( "Execution: mocked stream") {
	std::stringstream stream;
	auto lexer = std::make_shared<Lexer>(stream);
	auto parser = std::make_shared<Parser>(lexer);


	SECTION( "List statement with for" ) {

		const char *program =
		"int main() {\n"
		"	int a = 5;\n"
		"	int b = 3;\n"
		"	int c = 3 + a;\n"
		// "	println c;\n"
		"}\n";

		std::string exText(program);

		stream << exText;

		auto tree = parser->run();
		tree->print();
		auto interpreter = std::make_shared<Interpreter>(tree);
		interpreter->execute();

		interpreter->printSymbolTables();
	}

}
