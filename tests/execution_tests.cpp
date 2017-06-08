#include "lib/catch.hpp"
#include "../src/Parser.hpp"
#include "../src/Interpreter.hpp"

TEST_CASE( "Execution: mocked stream") {
	std::stringstream stream;
	auto lexer = std::make_shared<Lexer>(stream);
	auto parser = std::make_shared<Parser>(lexer);


	SECTION( "List statement with for" ) {

		// "int main() {\n"
		// "	list(int) l[10];\n"
		// "	[0 for i in l];\n"
		// "	int c = 5;\n"
		// "	for (int a = 0; a < 10; a = a + 1;) {\n"
		// "		l[a] = a;"
		// "		println l[a];\n"
		// "	}\n"
		// "}";

		const char *program =
		"int main() {\n"
			"int a = 7;\n"
			"int b = 6;\n"
			"bool greater = false;\n"
			"if (a > b)\n"
				"greater = true;\n"
			"\n"

			"println greater;\n"
		"}";

		std::string exText(program);

		stream << exText;

		auto tree = parser->run();
		tree->print();
		auto interpreter = std::make_shared<Interpreter>(tree);
		// tree->printSymbolTables();
		interpreter->execute();

		std::cout << "#5 Simple for statement" << std::endl;
		interpreter->printSymbolTables();
	}

}
