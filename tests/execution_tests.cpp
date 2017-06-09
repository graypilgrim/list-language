#include "lib/catch.hpp"
#include "../src/Parser.hpp"
#include "../src/Interpreter.hpp"

TEST_CASE( "Execution: mocked stream") {
	std::stringstream stream;
	auto lexer = std::make_shared<Lexer>(stream);
	auto parser = std::make_shared<Parser>(lexer);


	SECTION( "List statement with for" ) {

		// const char *program =
		// "int main() {\n"
		// "	int a = 5;\n"
		// "	int b = 3;\n"
		// "	int c = a + b + 9;\n"
		// "	println c;\n"
		// "}\n";

		// const char *program =
		// "int main() {\n"
		// "	int a = 8;\n"
		// "	int b = 6;\n"
		// "	bool greater = false;\n"
		// "	if (a > b) {\n"
		// "		greater = true;\n"
		// "	}\n"
		// "	println greater;\n"
		// "}\n";
		//
		// const char *program =
		// "int main() {\n"
		// "	int a = 1;\n"
		// "	for (int i = 0; i < 8; i = i + 1;) {\n"
		// "		println a;\n"
		// "		a = a * 2;\n"
		// "	}\n"
		// "	println a;\n"
		// "}\n";

		const char *program =
		"int main() {\n"
		"	list(int) l[10];\n"
		"	[3 for i in l];\n"
		"	[3 * i for i in l];\n"
		"	println l[3];\n"
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
