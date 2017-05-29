#include "lib/catch.hpp"
#include "../src/Parser.hpp"

TEST_CASE( "Parser: mocked stream") {
	auto lexer = std::make_shared<Lexer>();
	auto parser = std::make_shared<Parser>(lexer);
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());


	SECTION( "FunctionDeclaration - correct") {
		std::string exText("int fun();");
		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "FunctionDeclaration - incorrect") {
		std::string exText("int fun()=;");
		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try {
			parser->run();
		} catch (std::domain_error &e) {
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == true );
	}

	SECTION( "FunctionDeclarationWithArgs - correct") {
		std::string exText("int fun(bool a, bool b);");
		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "ListDeclaration - correct") {
		std::string exText("int fun() { list(bool) var; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "TwoVariablesDeclarations - incorrect") {
		std::string exText("int fun() { list(bool) var; int b = 314}" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try {
			parser->run();
		} catch (std::domain_error &e) {
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == true );
	}

	SECTION( "TwoVariablesDeclarations - correct") {
		std::string exText("int fun() { list(bool) var; float b = 3.14;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "IfStatement - correct") {
		std::string exText("int fun() { if (a == b) a = b;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "IfElseStatement - correct") {
		std::string exText("int fun() { if (a == b) a = b; else a = 1;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "IfStatementWithBrackets - correct") {
		std::string exText("int fun() { if (a == b) {a = b;} }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "whileStatement- correct") {
		std::string exText("int fun() { while(true) a = a + 1;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "ListStatement - correct") {
		std::string exText("int fun() { list(bool) var; [i + 3 for i in a]; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "ReturnStatement - correct") {
		std::string exText("int fun() { return 2; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "CallStatement - correct") {
		std::string exText("int fun() { foo(a, b, c); }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "IndexStatement - correct") {
		std::string exText("int fun() { table[i + 4]; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}

	SECTION( "ForLoop - correct") {
		std::string exText("int fun() { for(int a = 0; b < 5; c = d + 2;) e = 3;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		auto tree = parser->run();
		tree->print();
	}
}
