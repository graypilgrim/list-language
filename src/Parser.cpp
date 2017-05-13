#include "Parser.hpp"

Parser::Parser()
	: keywords({"list", "if", "else", "while", "for", "in", "return", "true", "false", "int", "float", "bool", "void"}),
	types({"int", "float", "bool", "void"})
{}

Parser::Parser(const std::shared_ptr<Lexer> &lexer)
	: lexer(lexer),
	  keywords({"list", "if", "else", "while", "for", "in", "return", "true", "false"}),
	  types({"int", "float", "bool", "void"})
{}

void Parser::run() {
}

void Parser::nextAtom() {
	atom = lexer->getNextAtom();
}

void Parser::program() {
	funsDecls();
}

void Parser::funsDefs() {

}

void Parser::funDef() {
	stmts();
}

void Parser::funsDecls() {
	funDecl();
}

void Parser::funDecl() {
	type();
	identifier();

	nextAtom();
	if (atom == ";")
		funsDecls();
	else if (atom == "=")
		varDef();
	else if (atom == "(")
		args();
	else
		throw std::domain_error("Not expected symbol at line: " + lexer->getLineNo());
}

void Parser::args() {
	type();
	identifier();

	nextAtom();
	if (atom == ",")
		args();
	else if (atom == ")")
		return;
	else
		throw std::domain_error("Closing paranthesis expected at line: " + lexer->getLineNo());
}

void Parser::stmts() {
	stmt();
}

void Parser::stmt() {
	nextAtom();

	if (atom == "if")
		condStmt();
	else if (atom == "for")
		forStmt();
	else if (atom == "while")
		whileStmt();
	else if (atom == "[")
		listStmt();
	else if (atom == "return")
		retStmt();
	else {
		auto it = types.find(atom);
		if (it != types.end())
			varDecl();
		else {
			identifier();
			auto atom = lexer->getNextAtom();

			if (atom == "[")
				indexStmt();
			else if (atom == "(")
				callStmt();
			else if (atom == "=")
				assignStmt();
		}
	}
}

void Parser::condStmt() {
	ifStmt();

	auto atom = lexer->getNextAtom();
	if (atom == "else")
		elseStmt();
	else {
		lexer->ungetAtom();
		stmts();
	}
}

void Parser::loppStmt() {

}

void Parser::whileStmt() {
	auto atom = lexer->getNextAtom();
	if (atom != "(")
		throw std::domain_error("Paranthesis expected at line " + lexer->getCurrentLine());

	expr();

	atom = lexer->getNextAtom();
	if (atom != ")")
		throw std::domain_error("Paranthesis expected at line " + lexer->getCurrentLine());

	atom = lexer->getNextAtom();
	if (atom == "{")
		stmts();
	else
		stmt();
}

void Parser::forStmt() {

}

void Parser::callStmt() {

}

void Parser::indexStmt() {

}

void Parser::varDef() {

}

void Parser::varDecl() {

}

void Parser::listStmt() {

}

void Parser::basicListIter() {

}

void Parser::listIfStmt() {

}

void Parser::retStmt() {

}

void Parser::assignStmt() {

}

void Parser::ifStmt() {

}

void Parser::elseStmt() {

}

void Parser::expr() {

}

void Parser::andExpr() {

}

void Parser::compExpr() {

}

void Parser::sumExpr() {

}

void Parser::mulExpr() {

}

void Parser::sumOperator() {

}

void Parser::mulOperator() {

}

void Parser::compOperator() {

}

void Parser::vals() {

}

void Parser::val() {

}

void Parser::lVal() {

}

void Parser::rVal() {

}

void Parser::type() {
	auto atom = lexer->getNextAtom();
	auto t = types.find(atom);

	if (t == types.end())
		throw std::domain_error("Type expected at line: " + lexer->getLineNo());
}

void Parser::identifier() {
	auto atom = lexer->getNextAtom();

	auto t = types.find(atom);
	auto id = keywords.find(atom);
	if (t != types.end() || id != keywords.end())
		throw std::domain_error("Identifier expected at line: " + lexer->getLineNo());
}

void Parser::intNumber() {

}

void Parser::floatNumber() {

}
