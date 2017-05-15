#include "Parser.hpp"

Parser::Parser()
	: keywords({"if", "else", "while", "for", "in", "return", "true", "false", "int", "float", "bool", "void"}),
	types({"list", "int", "float", "bool", "void"})
{}

Parser::Parser(const std::shared_ptr<Lexer> &lexer)
	: lexer(lexer),
	  keywords({"if", "else", "while", "for", "in", "return", "true", "false"}),
	  types({"list", "int", "float", "bool", "void"})
{}

void Parser::run() {
	program();
}

void Parser::nextAtom() {
	atom = lexer->getNextAtom();
}

void Parser::program() {
	nextAtom();
	funsDecls();
}

void Parser::funsDefs() {

}

void Parser::funDef() {
	stmts();
}

void Parser::funsDecls() {
	while (atom != "")
		funDecl();
}

void Parser::funDecl() {
	type();
	identifier();

	if (atom == ";") {
		nextAtom();
		funsDecls();
	} else if (atom == "=") {
		nextAtom();
		varDef();
	} else if (atom == "(") {
		nextAtom();
		if (atom != ")")
			args();
		else
			nextAtom();
	} else
		throw std::domain_error("Not expected symbol at line: " + std::to_string(lexer->getLineNo()));

	if (atom == "{") {
		funDef();
	} else if (atom == ";") {
		nextAtom();
		return;
	} else
		throw std::domain_error("Not expected symbol at line: " + std::to_string(lexer->getLineNo()));
}

void Parser::args() {
	type();
	identifier();

	if (atom == ",") {
		nextAtom();
		args();
	} else if (atom == ")") {
		nextAtom();
	} else
		throw std::domain_error("Closing paranthesis expected at line: " + std::to_string(lexer->getLineNo()));
}

void Parser::stmts() {
	// std::cout << ">>>>DEBUG: " << __FUNCTION__ << ": " << atom << std::endl;
	if (atom == "{"){
		nextAtom();
		while (atom != "}")
			stmt();
		nextAtom();
	} else
		stmt();
}

void Parser::stmt() {
	if (atom == "if") {
		nextAtom();
		condStmt();
	} else if (atom == "for") {
		nextAtom();
		forStmt();
	} else if (atom == "while") {
		nextAtom();
		whileStmt();
	} else if (atom == "[") {
		nextAtom();
		listStmt();
	} else if (atom == "return") {
		nextAtom();
		retStmt();
	} else {
		auto it = types.find(atom);
		if (it != types.end())
			varDecl();
		else {
			identifier();

			if (atom == "[") {
				nextAtom();
				indexStmt();
			} else if (atom == "(") {
				nextAtom();
				callStmt();
			} else if (atom == "=") {
				nextAtom();
				assignStmt();
			} else
				throw std::domain_error("Unexpected symbol at line: " + std::to_string(lexer->getLineNo()));
		}
	}
}

void Parser::condStmt() {
	ifStmt();

	if (atom == "else")
		elseStmt();
	else {
		stmts();
	}
}

void Parser::loppStmt() {

}

void Parser::whileStmt() {
	if (atom != "(")
		throw std::domain_error("Paranthesis expected at line " + lexer->getLineNo());
	nextAtom();

	expr();

	if (atom != ")")
		throw std::domain_error("Paranthesis expected at line " + lexer->getLineNo());
	nextAtom();

	stmts();
}

void Parser::forStmt() {

}

void Parser::callStmt() {

}

void Parser::indexStmt() {

}

void Parser::varDef() {
	if (isAtomNumber()) {
		nextAtom();
	} else if (isAtomBoolVal()) {
		nextAtom();
	} else
		identifier();

	if (atom != ";")
		throw std::domain_error("Not expected symbol at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
}

void Parser::varDecl() {
	type();
	identifier();

	if (atom == "=") {
		nextAtom();
		varDef();
	} else if (atom == ";") {
		nextAtom();
	} else
		throw std::domain_error("Not expected symbol at line: " + std::to_string(lexer->getLineNo()));
}

void Parser::listStmt() {
	basicListIter();

	if (atom == "if") {
		nextAtom();
		listIfStmt();
	}

	if (atom != "]")
		throw std::domain_error("Closing bracket expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
	if (atom != ";")
		throw std::domain_error("Semicolon expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
}

void Parser::basicListIter() {
	expr();
	if (atom != "for")
		throw std::domain_error("Iteration statement expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
	identifier();

	if (atom != "in")
		throw std::domain_error("\"in\" expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
	identifier();
}

void Parser::listIfStmt() {
	ifStmt();
}

void Parser::retStmt() {

}

void Parser::assignStmt() {
	if (atom == "["){
		nextAtom();
		listStmt();
	} else if (atom == "(") {
		nextAtom();
		callStmt();
	} else
		expr();

	if (atom != ";")
		throw std::domain_error("Semicolon expected at line: " + std::to_string(lexer->getLineNo()));
	else
		nextAtom();
}

void Parser::ifStmt() {
	if (atom != "(")
		throw std::domain_error("Opening paranthesis expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
	expr();
	if (atom != ")")
		throw std::domain_error("Closing paranthesis expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
}

void Parser::elseStmt() {

}

void Parser::expr() {
	andExpr();
	if (atom == "||") {
		nextAtom();
		andExpr();
	}
}

void Parser::andExpr() {
	compExpr();
	if (atom == "&&") {
		nextAtom();
		compExpr();
	}
}

void Parser::compExpr() {
	sumExpr();
	if (isAtomCompOperator()) {
		nextAtom();
		sumExpr();
	}
}

void Parser::sumExpr() {
	mulExpr();
	if (isAtomSumOperator()) {
		nextAtom();
		mulExpr();
	}
}

void Parser::mulExpr() {
	val();
	if (isAtomMulOperator()) {
		nextAtom();
		val();
	}
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

	if (isAtomNumber() || isAtomBoolVal()) {
		nextAtom();
	} else {
		identifier();
		if (atom == "(") {
			nextAtom();
			callStmt();
		} else if (atom == "[") {
			nextAtom();
			indexStmt();
		}
	}
}

void Parser::lVal() {

}

void Parser::rVal() {
}

void Parser::type() {
	auto t = types.find(atom);

	if (t == types.end())
		throw std::domain_error("Type expected at line: " + std::to_string(lexer->getLineNo()));

	if (atom == "list") {
		nextAtom();
		if (atom != "(")
			throw std::domain_error("Opening paranthesis expected at line: " + std::to_string(lexer->getLineNo()));

		nextAtom();
		type();
		if (atom != ")")
			throw std::domain_error("Closing paranthesis expected at line: " + std::to_string(lexer->getLineNo()));
	}

	nextAtom();
}

void Parser::identifier() {
	auto t = types.find(atom);
	auto id = keywords.find(atom);
	if (t != types.end() || id != keywords.end())
		throw std::domain_error("Identifier expected at line: " + std::to_string(lexer->getLineNo()));

	nextAtom();
}

void Parser::intNumber() {

}

void Parser::floatNumber() {

}

bool Parser::isAtomNumber() {
	return atom.size() > 0 && atom[0] >= '0' && atom[0] <= '9';
}

bool Parser::isAtomBoolVal() {
	return atom == "true" || atom == "false";
}

bool Parser::isAtomCompOperator() {
	return atom == "<" || atom == ">" || atom == "==" || atom == "<=" || atom == ">=";
}

bool Parser::isAtomSumOperator() {
	return atom == "+" || atom == "-";
}

bool Parser::isAtomMulOperator() {
	return atom == "*" || atom == "/";
}
