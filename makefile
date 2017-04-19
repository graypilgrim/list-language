.DEFAULT_GOAL := all

CXX = g++
CXXFLAGS = -c -Wall -pedantic -o2
BINARY = main.out
TESTS_BINARY = tests_main.out

SOURCES = \
	./src/main.cpp \
	./src/Lexer.cpp \

OBJECTS=$(SOURCES:.cpp=.o)

$(BINARY): $(OBJECTS)
	$(CXX) $^ -o $@


TESTS_SOURCES = \
	./tests/tests_main.cpp \
	./src/Lexer.cpp \
	./tests/lexer_tests.cpp \

TEST_OBJECTS=$(TESTS_SOURCES:.cpp=.o)

$(TESTS_BINARY): $(TEST_OBJECTS)
	$(CXX) $^ -o $@

all : tests

tests : $(TESTS_BINARY)
	./$(TESTS_BINARY)

prod : $(BINARY)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	@rm -f $(BINARY)
	@rm -f $(TESTS_BINARY)
	@rm -f src/*.o
	@rm -f src/*.gch
	@rm -f tests/*.o

.PHONY:
	clean, all
