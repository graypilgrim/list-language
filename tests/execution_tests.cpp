#include "lib/catch.hpp"
#include "../src/Parser.hpp"

TEST_CASE( "Execution: mocked stream") {
	auto lexer = std::make_shared<Lexer>();
	auto parser = std::make_shared<Parser>(lexer);
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	SECTION( "Simple sum") {
		const char *program =
		"int main() {\n"
		"	int a = 5;\n"
		"	int b = 3;\n"
		"	int c = a + b + 3;\n"
		"}";

		std::string exText(program);

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->execute();
		std::cout << "#1 Simple sum statement" << std::endl;
		tree->printSymbolTables();
	}

	SECTION( "Simple if statement") {
		const char *program =
		"int main() {\n"
		"	int a = 7;\n"
		"	int b = 6;\n"
		"	bool greater = false;\n"
		"	if (a > b)\n"
		"		greater = true;\n"
		"}";

		std::string exText(program);

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		// tree->printTree();
		tree->execute();
		// tree->printTree();
		std::cout << "#2 Simple if statement" << std::endl;
		tree->printSymbolTables();
	}

	SECTION( "Simple for statement" ) {
		const char *program =
		"int main() {\n"
		"	int a = 1;\n"
		"	for (int i = 0; i < 8; i = i + 1;) {\n"
		"		a = a * 2;\n"
		"	}\n"
		"}";

		std::string exText(program);

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		// tree->printTree();
		// tree->printSymbolTables();
		tree->execute();

		std::cout << "#3 Simple for statement" << std::endl;
		tree->printSymbolTables();
	}

	SECTION( "Simple list statement" ) {
		const char *program =
		"int main() {\n"
		"	list(int) l[10];\n"
		"	[i + 3 for i in l];\n"
		"}";

		std::string exText(program);

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->printTree();
		// tree->printSymbolTables();
		tree->execute();

		std::cout << "#4 Simple for statement" << std::endl;
		tree->printSymbolTables();
	}

}
