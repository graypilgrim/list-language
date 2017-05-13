#include <stdlib.h>

#include "lib/catch.hpp"
#include "../src/Lexer.hpp"

TEST_CASE( "StreamSetting" ) {
	Lexer lexer;
	REQUIRE( lexer.isStreamSet() == false );

	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	lexer.setStream(exampleStream);

	REQUIRE( lexer.isStreamSet() == true );
}

TEST_CASE( "Working with mocked stream" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	SECTION( "OneLineReading" ) {
		std::string exText("hello world");
		*s << exText;
		lexer.setStream(exampleStream);

		REQUIRE( exampleStream.use_count() == 2 );
		REQUIRE( lexer.getCurrentLine() == exText );
	}

	SECTION( "OneWord" ) {
		std::string exText("hello");
		*s << exText;
		lexer.setStream(exampleStream);
		auto result = lexer.getNextAtom();

		REQUIRE( result == exText );
	}

	SECTION( "TwoWord" ) {
		std::string exText1("hello");
		std::string exText2("hello");
		*s << exText1 << " " << exText2;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
	}

	SECTION( "WordAndNumber" ) {
		std::string exText1("hello");
		std::string exText2("1234");
		*s << exText1 << " " << exText2;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
	}

	SECTION( "VariableDefinition" ) {
		std::string exText1("int");
		std::string exText2("a");
		std::string exText3("=");
		std::string exText4("123.123");
		std::string exText5(";");
		*s << exText1 << " " << exText2 << exText3 << exText4 << exText5;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();
		auto result3 = lexer.getNextAtom();
		auto result4 = lexer.getNextAtom();
		auto result5 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
		REQUIRE( result3 == exText3 );
		REQUIRE( result4 == exText4 );
		REQUIRE( result5 == exText5 );
	}

	SECTION( "InvalidNumber" ) {
		std::string exText("123hello");
		*s << exText;
		lexer.setStream(exampleStream);

		bool exceptionCaught = false;

		try
		{
			lexer.getNextAtom();
		}
		catch (std::domain_error &e)
		{
			exceptionCaught = true;
		}

		REQUIRE( exceptionCaught == true );
	}

	SECTION( "LogicTies" ) {
		std::string exText1("a");
		std::string exText2("&&");
		std::string exText3("b");
		*s << exText1 << " " << exText2 << " " << exText3;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();
		auto result3 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
		REQUIRE( result3 == exText3 );
	}

	SECTION( "TwoWordsInTwoLines" ) {
		std::string exText1("a");
		std::string exText2("b");
		*s << exText1 << "\n" << exText2;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
		REQUIRE( lexer.getLineNo() == 2);
	}

	SECTION( "VarInitializationAndComment" ) {
		std::string exText1("void");
		std::string exText2("a");
		std::string exText3("@asdasd");
		*s << exText1 << " " << exText2 << exText3;
		lexer.setStream(exampleStream);
		auto result1 = lexer.getNextAtom();
		auto result2 = lexer.getNextAtom();
		auto result3 = lexer.getNextAtom();

		REQUIRE( result1 == exText1 );
		REQUIRE( result2 == exText2 );
		REQUIRE( result3 == "");
	}
}
