.DEFAULT_GOAL := all

CXX = g++
CXXFLAGS = -c -Wall -Wpedantic -O2
BINARY = main.out
TESTS_BINARY = tests_main.out

SOURCES = \
	./src/Lexer.cpp \
	./src/GlobalItem.cpp \
	./src/Parser.cpp \

PROD_SOURCES = \
	$(SOURCES) \
	./src/main.cpp \

PROD_OBJECTS=$(PROD_SOURCES:.cpp=.o)

$(BINARY): $(OBJECTS)
	$(CXX) $^ -o $@


TESTS_SOURCES = \
	$(SOURCES) \
	./tests/tests_main.cpp \
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
