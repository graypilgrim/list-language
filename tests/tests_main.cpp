#define CATCH_CONFIG_MAIN
#include "lib/catch.hpp"

#include "../src/Lexer.hpp"

TEST_CASE( "Factorials are computed", "[factorial]" ) {
	Lexer::instance().openFile("tests_main.cpp");
}
