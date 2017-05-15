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

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "FunctionDeclaration - incorrect") {
		std::string exText("int fun()=;");
		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;
		std::domain_error exception("");

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exception = std::move(e);
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == true );
		REQUIRE( exception.what() == std::string("Semicolon expected at line: 1") );
	}

	SECTION( "FunctionDeclarationWithArgs - correct") {
		std::string exText("int fun(bool a, bool b);");
		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "ListDeclaration - correct") {
		std::string exText("int fun() { list(bool) var; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "TwoVariablesDeclarations - incorrect") {
		std::string exText("int fun() { list(bool) var; int b = 314}" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == true );
	}

	SECTION( "TwoVariablesDeclarations - correct") {
		std::string exText("int fun() { list(bool) var; float b = 3.14;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "IfStatement - correct") {
		std::string exText("int fun() { if (a == b) a = b;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "IfStatementWithBrackets - correct") {
		std::string exText("int fun() { if (a == b) {a = b;} }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "whileStatement- correct") {
		std::string exText("int fun() { while(true) a = a + 1;}" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "ListStatement - correct") {
		std::string exText("int fun() { list(bool) var; [i + 3 for i in a]; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
			std::cout << e.what() << std::endl;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "ReturnStatement - correct") {
		std::string exText("int fun() { return 2; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
			std::cout << e.what() << std::endl;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "CallStatement - correct") {
		std::string exText("int fun() { foo(a, b, c); }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
			std::cout << e.what() << std::endl;
		}

		REQUIRE( exceptionCaught == false );
	}

	SECTION( "IndexStatement - correct") {
		std::string exText("int fun() { table[i + 4]; }" );

		*s << exText;
		lexer->setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			parser->run();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
			std::cout << e.what() << std::endl;
		}

		REQUIRE( exceptionCaught == false );
	}
}
