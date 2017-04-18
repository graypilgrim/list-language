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

TEST_CASE( "ReadingOneLine" ) {
	Lexer lexer;
	std::shared_ptr<std::istream> exampleStream(new std::stringstream);
	auto s = static_cast<std::stringstream*>(exampleStream.get());

	std::string exText("hello world");
	*s << exText;
	lexer.setStream(exampleStream);

	REQUIRE( exampleStream.use_count() == 2 );
	REQUIRE( lexer.getCurrentLine() == exText );
}
