#ifndef PARSER_HPP
#define PARSER_HPP

#include "GlobalItem.hpp"
#include "Lexer.hpp"

#include <vector>
#include <set>
#include <unordered_map>
#include <memory>
#include <stdexcept>

class Parser
{
public:
	Parser();
	Parser(const std::shared_ptr<Lexer> &lexer);
	void run();

private:
	void nextAtom();

	void program();
	void funsDefs();
	void funDef();
	void funsDecls();
	void funDecl();
	void args();
	void stmts();
	void stmt();
	void condStmt();
	void loppStmt();
	void whileStmt();
	void forStmt();
	void callStmt();
	void indexStmt();
	void varDef();
	void varDecl();
	void listStmt();
	void basicListIter();
	void listIfStmt();
	void retStmt();
	void assignStmt();
	void ifStmt();
	void elseStmt();
	void expr();
	void andExpr();
	void compExpr();
	void sumExpr();
	void mulExpr();
	void sumOperator();
	void mulOperator();
	void compOperator();
	void vals();
	void val();
	void lVal();
	void rVal();
	void type();
	void identifier();
	void intNumber();
	void floatNumber();

	void semicolon();

	inline bool isAtomNumber();
	inline bool isAtomBoolVal();
	inline bool isAtomCompOperator();
	inline bool isAtomSumOperator();
	inline bool isAtomMulOperator();

	std::string atom;

	std::shared_ptr<Lexer> lexer;
	std::set<std::string> keywords;
	std::set<std::string> types;
	std::unordered_map<std::string, size_t> globalItems;
};

#endif
