#ifndef PARSER_HPP
#define PARSER_HPP

#include "GlobalItem.hpp"
#include "Lexer.hpp"

#include <vector>
#include <memory>
#include <stdexcept>

class Parser
{
	Parser() = default;
	Parser(const std::shared_ptr<Lexer> &lexer);
	void run();

private:
	void program();
	void funsDefs();
	void funDef();
	void fundDecls();
	void fundDecl();
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

	//TODO: keyword collection
	std::vector<GlobalItem> globalItems;
	std::shared_ptr<Lexer> lexer;
};

#endif
