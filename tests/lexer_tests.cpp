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

TEST_CASE( "OneWordReading" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText("hello");
	*s << exText;
	lexer.setStream(exampleStream);
	auto result = lexer.getNextAtom();

	REQUIRE( result == exText );
}

TEST_CASE( "TwoWordReading" ) {
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

TEST_CASE( "WordAndNumberReading" ) {
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
