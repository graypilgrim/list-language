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
		tree->printTree();
		tree->printSymbolTables();
		tree->execute();
		tree->printSymbolTables();
	}

}
