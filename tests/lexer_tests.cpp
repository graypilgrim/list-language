#include <stdlib.h>

#include "lib/catch.hpp"
#include "../src/Lexer.hpp"

TEST_CASE( "FileOpening" ) {
	Lexer lexer;
	REQUIRE( lexer.isStreamSet() == false );

	lexer.openFile(getenv("_"));

	REQUIRE( lexer.isStreamSet() == true );
}

TEST_CASE( "StreamSetting" ) {
	Lexer lexer;
	REQUIRE( lexer.isStreamSet() == false );

	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	lexer.setStream(exampleStream);

	REQUIRE( lexer.isStreamSet() == true );
}

TEST_CASE( "OneLineReading" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText("hello world");
	*s << exText;
	lexer.setStream(exampleStream);

	REQUIRE( exampleStream.use_count() == 2 );
	REQUIRE( lexer.getCurrentLine() == exText );
}

TEST_CASE( "OneWord" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText("hello");
	*s << exText;
	lexer.setStream(exampleStream);
	auto result = lexer.getNextAtom();

	REQUIRE( result == exText );
}

TEST_CASE( "TwoWord" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText1("hello");
	std::string exText2("hello");
	*s << exText1 << " " << exText2;
	lexer.setStream(exampleStream);
	auto result1 = lexer.getNextAtom();
	auto result2 = lexer.getNextAtom();

	REQUIRE( result1 == exText1 );
	REQUIRE( result2 == exText2 );
}

TEST_CASE( "WordAndNumber" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText1("hello");
	std::string exText2("1234");
	*s << exText1 << " " << exText2;
	lexer.setStream(exampleStream);
	auto result1 = lexer.getNextAtom();
	auto result2 = lexer.getNextAtom();

	REQUIRE( result1 == exText1 );
	REQUIRE( result2 == exText2 );
}

TEST_CASE( "VariableDefinition" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

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

TEST_CASE( "InvalidNumber" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText("123hello");
	*s << exText;
	lexer.setStream(exampleStream);

	bool exceptionCaught = false;

	try
	{
		lexer.getNextAtom();
	}
	catch (...)
	{
		exceptionCaught = true;
	}

	REQUIRE( exceptionCaught == true );
}

TEST_CASE( "LogicTies" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

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
