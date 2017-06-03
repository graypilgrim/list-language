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

std::shared_ptr<DerivationTree> Parser::run() {
	auto root = std::make_shared<DerivationNode>("program");
	tree = std::make_shared<DerivationTree>(root);
	tree->pushTable(root->createScope());
	nextAtom(root);
	root->addChild(funsDecls(root));

	return tree;
}

std::shared_ptr<DerivationNode> Parser::nextAtom(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(atom, parent);
	atom = lexer->getNextAtom();
	return node;
}

std::string Parser::currentLine() {
	return std::to_string(lexer->getLineNo());
}

std::shared_ptr<DerivationNode> Parser::funDef(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("funDef", parent);
	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::funsDecls(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("funDecls", parent);
	while (atom != "")
		node->addChild(funDecl(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::funDecl(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("funDecl", parent);
	node->addChild(type(node));
	node->addChild(identifier(node));

	if (atom == ";") {
		node->addChild(nextAtom(node));
		return node;
	} else if (atom == "=") {
		node->addChild(nextAtom(node));
	} else if (atom == "(") {
		node->addChild(nextAtom(node));
		if (atom != ")") {
			tree->pushTable(node->createScope());
			node->addChild(args(node));
		} else
			node->addChild(nextAtom(node));
	} else
		throw std::domain_error(std::string(__FUNCTION__) + " Not expected symbol at line: " + currentLine());

	if (atom == "{") {
		node->addChild(funDef(node));
	} else
		node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::args(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("args", parent);
	node->addChild(type(node));
	node->addChild(identifier(node));

	if (atom == ",") {
		node->addChild(nextAtom(node));
		node->addChild(args(node));
	} else if (atom == ")") {
		node->addChild(nextAtom(node));
	} else
		throw std::domain_error(std::string(__FUNCTION__) + " Closing paranthesis expected at line: " + currentLine());

	return node;
}

std::shared_ptr<DerivationNode> Parser::stmts(const std::weak_ptr<DerivationNode> &parent) {
	// std::cout << ">>>>DEBUG: " << __FUNCTION__ << ": " << atom << std::endl;
	auto node = std::make_shared<DerivationNode>("stmts", parent);
	if (atom == "{"){
		node->addChild(nextAtom(node));
		tree->pushTable(node->createScope());

		while (atom != "}")
			node->addChild(stmt(node));

		node->addChild(nextAtom(node));

	} else
		node->addChild(stmt(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::stmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("stmts", parent);

	if (atom == "if") {
		node->addChild(nextAtom(node));
		node->addChild(condStmt(node));
	} else if (atom == "for") {
		node->addChild(nextAtom(node));
		node->addChild(forStmt(node));
	} else if (atom == "while") {
		node->addChild(nextAtom(node));
		node->addChild(whileStmt(node));
	} else if (atom == "[") {
		node->addChild(nextAtom(node));
		node->addChild(listStmt(node));
	} else if (atom == "return") {
		node->addChild(nextAtom(node));
		node->addChild(retStmt(node));
	} else {
		if (isAtomType())
			node->addChild(varDecl(node));
		else {
			node->addChild(identifier(node));

			if (atom == "[") {
				node->addChild(nextAtom(node));
				node->addChild(indexStmt(node));
			} else if (atom == "(") {
				node->addChild(nextAtom(node));
				node->addChild(callStmt(node));
			} else if (atom == "=") {
				node->addChild(nextAtom(node));
				node->addChild(assignStmt(node));
			} else
				throw std::domain_error(std::string(__FUNCTION__) + " Unexpected symbol at line: " + currentLine());
		}
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::condStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("condStmt", parent);

	node->addChild(ifStmt(node));
	node->addChild(stmts(node));

	if (atom == "else") {
		node->addChild(nextAtom(node));
		node->addChild(elseStmt(node));
		node->addChild(stmts(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::whileStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("whileStmt", parent);

	if (atom != "(")
		throw std::domain_error(std::string(__FUNCTION__) + " Paranthesis expected at line " + currentLine());
	node->addChild(nextAtom(node));

	node->addChild(expr(node));

	if (atom != ")")
		throw std::domain_error(std::string(__FUNCTION__) + " Paranthesis expected at line " + currentLine());
	node->addChild(nextAtom(node));

	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::forStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("forStmt", parent);

	if (atom != "(")
		throw std::domain_error(std::string(__FUNCTION__) + " Paranthesis expected at line " + currentLine());
	node->addChild(nextAtom(node));

	if (atom == ";") {
		node->addChild(nextAtom(node));
	} else {
		if (isAtomType()) {
			node->addChild(varDecl(node));
		} else {
			node->addChild(identifier(node));
			if (atom == "=")
				node->addChild(assignStmt(node));
		}
	}

	if (atom == ";") {
		node->addChild(nextAtom(node));
	} else {
		node->addChild(expr(node));
	}

	node->addChild(semicolon(node));
	node->addChild(stmt(node));
	if (atom != ")")
		throw std::domain_error(std::string(__FUNCTION__) + " Closing paranthesis expected at line " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::callStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("callStmt", parent);

	if (atom == ")") {
		node->addChild(nextAtom(node));
		return node;
	}

	while (atom != ""){
		node->addChild(expr(node));

		if (atom == ",") {
			node->addChild(nextAtom(node));
		} else if (atom == ")") {
			node->addChild(nextAtom(node));
			break;
		} else
			throw std::domain_error(std::string(__FUNCTION__) + " Unexpected symbol at line: " + currentLine());
	}

	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::indexStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("indexStmt", parent);

	node->addChild(expr(node));
	if (atom != "]")
		throw std::domain_error(std::string(__FUNCTION__) + " Unexpected symbol at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::varDef(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("varDef", parent);

	if (isAtomNumber()) {
		node->addChild(nextAtom(node));
	} else if (isAtomBoolVal()) {
		node->addChild(nextAtom(node));
	} else
		node->addChild(identifier(node));

	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::varDecl(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("varDecl", parent);

	node->addChild(type(node));
	node->addChild(identifier(node));

	if (atom == "=") {
		node->addChild(nextAtom(node));
		node->addChild(varDef(node));
	} else if (atom == ";") {
		node->addChild(nextAtom(node));
	} else if (atom == "[") {
		node->addChild(nextAtom(node));
		node->addChild(indexStmt(node));
	} else
		throw std::domain_error(std::string(__FUNCTION__) + " Unexpected symbol at line: " + currentLine());

	return node;
}

std::shared_ptr<DerivationNode> Parser::listStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("listStmt", parent);

	node->addChild(basicListIter(node));

	if (atom == "if") {
		node->addChild(nextAtom(node));
		node->addChild(listIfStmt(node));
	}

	if (atom != "]")
		throw std::domain_error(std::string(__FUNCTION__) + " Closing bracket expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::basicListIter(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("basicListIter", parent);

	node->addChild(expr(node));
	if (atom != "for")
		throw std::domain_error(std::string(__FUNCTION__) + " Iteration statement expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(identifier(node));

	if (atom != "in")
		throw std::domain_error(std::string(__FUNCTION__) + " \"in\" expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(identifier(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::listIfStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("funDecl", parent);
	node->addChild(ifStmt(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::retStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("funDecl", parent);
	node->addChild(expr(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::assignStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("assignStmt", parent);

	if (atom == "["){
		node->addChild(nextAtom(node));
		node->addChild(listStmt(node));
	} else if (atom == "(") {
		node->addChild(nextAtom(node));
		node->addChild(callStmt(node));
	} else
		node->addChild(expr(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::ifStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("ifStmt", parent);

	if (atom != "(")
		throw std::domain_error(std::string(__FUNCTION__) + " Opening paranthesis expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(expr(node));
	if (atom != ")")
		throw std::domain_error(std::string(__FUNCTION__) + " Closing paranthesis expected at line: " + currentLine());

	node->addChild(nextAtom(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::elseStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("elseStmt", parent);

	if (atom == "if") {
		node->addChild(nextAtom(node));
		node->addChild(condStmt(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::expr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("expr", parent);

	node->addChild(andExpr(node));
	if (atom == "||") {
		node->addChild(nextAtom(node));
		node->addChild(andExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::andExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("andExpr", parent);

	node->addChild(compExpr(node));
	if (atom == "&&") {
		node->addChild(nextAtom(node));
		node->addChild(compExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::compExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("comExpr", parent);

	node->addChild(sumExpr(node));
	if (isAtomCompOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(sumExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::sumExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("sumExpr", parent);

	node->addChild(mulExpr(node));
	if (isAtomSumOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(mulExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::mulExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("mulExpr", parent);

	node->addChild(val(node));
	if (isAtomMulOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(val(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::val(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("val", parent);

	if (isAtomNumber() || isAtomBoolVal()) {
		node->addChild(nextAtom(node));
	} else {
		node->addChild(identifier(node));
		if (atom == "(") {
			node->addChild(nextAtom(node));
			node->addChild(callStmt(node));
		} else if (atom == "[") {
			node->addChild(nextAtom(node));
			node->addChild(indexStmt(node));
		}
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::type(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("type", parent);

	auto t = types.find(atom);

	if (t == types.end())
		throw std::domain_error(std::string(__FUNCTION__) + " Type expected at line: " + currentLine());

	if (atom == "list") {
		node->addChild(nextAtom(node));
		if (atom != "(")
			throw std::domain_error(std::string(__FUNCTION__) + " Opening paranthesis expected at line: " + currentLine());

		node->addChild(nextAtom(node));
		node->addChild(type(node));
		if (atom != ")")
			throw std::domain_error(std::string(__FUNCTION__) + " Closing paranthesis expected at line: " + currentLine());
	}

	node->addChild(nextAtom(node));
	return node;
}

std::shared_ptr<DerivationNode> Parser::identifier(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("identifier", parent);

	auto t = types.find(atom);
	auto id = keywords.find(atom);
	if (t != types.end() || id != keywords.end())
		throw std::domain_error(std::string(__FUNCTION__) + " Identifier expected at line: " + currentLine());

	auto table = node->getSymbolTable();
	auto entry = std::make_shared<SymbolTableEntry>();
	table->addEntry(atom, entry, node);

	node->addChild(nextAtom(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::semicolon(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>("semicolon", parent);

	if (atom != ";")
		throw std::domain_error(std::string(__FUNCTION__) + " Semicolon expected at line: " + currentLine());

	node->addChild(nextAtom(node));

	return node;
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

bool Parser::isAtomType() {
	auto it = types.find(atom);
	return it != types.end();
}
