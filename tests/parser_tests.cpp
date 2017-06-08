#include "lib/catch.hpp"
#include "../src/Parser.hpp"

TEST_CASE( "Parser: mocked stream") {
	std::stringstream stream;
	auto lexer = std::make_shared<Lexer>(stream);
	auto parser = std::make_shared<Parser>(lexer);

	SECTION( "FunctionDeclaration - correct") {
		std::string exText("int fun();");
		stream << exText;
		auto tree = parser->run();
	}

	SECTION( "FunctionDeclaration - incorrect") {
		std::string exText("int fun()=;");
		stream << exText;

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
		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "ListDeclaration - correct") {
		std::string exText("int fun() { list(bool) var; }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "TwoVariablesDeclarations - incorrect") {
		std::string exText("int fun() { list(bool) var; int b = 314}" );

		stream << exText;


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

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "IfStatement - correct") {
		std::string exText("int fun() { if (a == b) a = b;}" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "IfElseStatement - correct") {
		std::string exText("int fun() { if (a == b) a = b; else a = 1;}" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "IfStatementWithBrackets - correct") {
		std::string exText("int fun() { if (a == b) {a = b;} }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "whileStatement- correct") {
		std::string exText("int fun() { while(true) a = a + 1;}" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "ListStatement - correct") {
		std::string exText("int fun() { list(bool) var; [i + 3 for i in a]; }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "ReturnStatement - correct") {
		std::string exText("int fun() { return 2; }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "CallStatement - correct") {
		std::string exText("int fun() { foo(a, b, c); }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "IndexStatement - correct") {
		std::string exText("int fun() { list(bool) table[i + 4]; }" );

		stream << exText;

		auto tree = parser->run();
	}

	SECTION( "ForLoop - correct") {
		std::string exText("int fun() { for(int a = 0; b < 5; c = d + 2;) e = 3;}" );

		stream << exText;

		auto tree = parser->run();
	}
}
