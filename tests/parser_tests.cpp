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
		REQUIRE( exception.what() == std::string("Not expected symbol at line: 1") );
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

		REQUIRE( exceptionCaught == true );
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
}
