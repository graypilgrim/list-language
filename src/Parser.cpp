#include "Parser.hpp"

Parser::Parser()
	: keywords({"if", "else", "while", "for", "in", "return", "true", "false", "int", "float", "bool", "void", "print", "println"}),
	types({"list", "int", "float", "bool", "void"})
{}

Parser::Parser(const std::shared_ptr<Lexer> &lexer)
	: lexer(lexer),
	  keywords({"if", "else", "while", "for", "in", "return", "true", "false"}),
	  types({"list", "int", "float", "bool", "void"})
{}

std::shared_ptr<DerivationTree> Parser::run() {
	auto root = std::make_shared<DerivationNode>(NodeType::program);
	tree = std::make_shared<DerivationTree>(root);
	tree->pushTable(root->createScope());
	nextAtom(root);
	root->addChild(funsDecls(root));

	return tree;
}

std::shared_ptr<DerivationNode> Parser::nextAtom(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::symbol, parent);
	node->setLabel(atom);
	atom = lexer->getNextAtom();
	return node;
}

std::string Parser::currentLine() {
	return std::to_string(lexer->getLineNo());
}

std::shared_ptr<DerivationNode> Parser::funDef(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::funDef, parent);
	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::funsDecls(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::funDecls, parent);
	while (atom != "")
		node->addChild(funDecl(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::funDecl(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::funDecl, parent);
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
		throw std::domain_error(" Not expected symbol at line: " + currentLine());

	if (atom == "{") {
		node->addChild(funDef(node));
	} else
		node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::args(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::args, parent);
	node->addChild(type(node));
	node->addChild(identifier(node));

	if (atom == ",") {
		node->addChild(nextAtom(node));
		node->addChild(args(node));
	} else if (atom == ")") {
		node->addChild(nextAtom(node));
	} else
		throw std::domain_error(" Closing paranthesis expected at line: " + currentLine());

	return node;
}

std::shared_ptr<DerivationNode> Parser::stmts(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::stmts, parent);
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
	auto node = std::make_shared<DerivationNode>(NodeType::stmt, parent);

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
	} else if (atom == "print" || atom == "println") {
		auto label = atom;
		node->addChild(nextAtom(node));
		node->addChild(printStmt(node, label));
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
				throw std::domain_error(" Unexpected symbol at line: " + currentLine());
		}
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::condStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::condStmt, parent);

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
	auto node = std::make_shared<DerivationNode>(NodeType::whileStmt, parent);

	if (atom != "(")
		throw std::domain_error(" Paranthesis expected at line " + currentLine());
	node->addChild(nextAtom(node));

	node->addChild(expr(node));

	if (atom != ")")
		throw std::domain_error(" Paranthesis expected at line " + currentLine());
	node->addChild(nextAtom(node));

	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::forStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::forStmt, parent);
	tree->pushTable(node->createScope());

	if (atom != "(")
		throw std::domain_error(" Paranthesis expected at line " + currentLine());
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
		throw std::domain_error(" Closing paranthesis expected at line " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(stmts(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::callStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::callStmt, parent);

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
			throw std::domain_error(" Unexpected symbol at line: " + currentLine());
	}

	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::indexStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::indexStmt, parent);

	node->addChild(expr(node));
	if (atom != "]")
		throw std::domain_error(" Unexpected symbol at line: " + currentLine());

	node->addChild(nextAtom(node));

	if (atom == "=") {
		node->addChild(nextAtom(node));
		node->addChild(assignStmt(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::varDef(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::varDef, parent);

	if (isAtomNumber()) {
		node->addChild(nextAtom(node));
	} else if (isAtomBoolVal()) {
		node->addChild(nextAtom(node));
	} else
		node->addChild(expr(node));

	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::varDecl(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::varDecl, parent);

	node->addChild(type(node));
	node->addChild(identifier(node));

	if (atom == "=") {
		node->addChild(nextAtom(node));
		node->addChild(assignStmt(node));
	} else if (atom == "[") {
		node->addChild(nextAtom(node));
		node->addChild(indexStmt(node));
		node->addChild(semicolon(node));
	} else
		node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::listStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::listStmt, parent);

	node->addChild(basicListIter(node));

	if (atom == "if") {
		node->addChild(nextAtom(node));
		node->addChild(listIfStmt(node));
	}

	if (atom != "]")
		throw std::domain_error(" Closing bracket expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::basicListIter(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::basicListIter, parent);

	node->addChild(expr(node));
	if (atom != "for")
		throw std::domain_error(" Iteration statement expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(identifier(node));

	if (atom != "in")
		throw std::domain_error(" \"in\" expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(identifier(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::listIfStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::funDecl, parent);
	node->addChild(ifStmt(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::retStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::funDecl, parent);
	node->addChild(expr(node));
	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::assignStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::assignStmt, parent);

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
	auto node = std::make_shared<DerivationNode>(NodeType::ifStmt, parent);

	if (atom != "(")
		throw std::domain_error(" Opening paranthesis expected at line: " + currentLine());

	node->addChild(nextAtom(node));
	node->addChild(expr(node));
	if (atom != ")")
		throw std::domain_error(" Closing paranthesis expected at line: " + currentLine());

	node->addChild(nextAtom(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::elseStmt(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::elseStmt, parent);

	if (atom == "if") {
		node->addChild(nextAtom(node));
		node->addChild(condStmt(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::printStmt(const std::weak_ptr<DerivationNode> &parent, const std::string &label) {
	auto node = std::make_shared<DerivationNode>(NodeType::printStmt, parent);
	node->setLabel(label);

	if (atom.size() > 0 && atom[0] == '"')
		node->addChild(nextAtom(node));
	else
		node->addChild(val(node));

	node->addChild(semicolon(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::expr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::expr, parent);

	node->addChild(andExpr(node));
	while (atom == "||") {
		node->addChild(nextAtom(node));
		node->addChild(andExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::andExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::andExpr, parent);

	node->addChild(compExpr(node));
	while (atom == "&&") {
		node->addChild(nextAtom(node));
		node->addChild(compExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::compExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::compExpr, parent);

	node->addChild(sumExpr(node));
	while (isAtomCompOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(sumExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::sumExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::sumExpr, parent);

	node->addChild(mulExpr(node));
	while (isAtomSumOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(mulExpr(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::mulExpr(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::mulExpr, parent);

	node->addChild(val(node));
	while (isAtomMulOperator()) {
		node->addChild(nextAtom(node));
		node->addChild(val(node));
	}

	return node;
}

std::shared_ptr<DerivationNode> Parser::val(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::val, parent);

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
	auto node = std::make_shared<DerivationNode>(NodeType::type, parent);

	auto t = types.find(atom);

	if (t == types.end())
		throw std::domain_error(" Type expected at line: " + currentLine());

	if (atom == "list") {
		node->addChild(nextAtom(node));
		if (atom != "(")
			throw std::domain_error(" Opening paranthesis expected at line: " + currentLine());

		node->addChild(nextAtom(node));
		node->addChild(type(node));
		if (atom != ")")
			throw std::domain_error(" Closing paranthesis expected at line: " + currentLine());
	}

	node->addChild(nextAtom(node));
	return node;
}

std::shared_ptr<DerivationNode> Parser::identifier(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::identifier, parent);

	auto t = types.find(atom);
	auto keyword = keywords.find(atom);
	if (t != types.end() || keyword != keywords.end())
		throw std::domain_error(" Identifier expected at line: " + currentLine());


	if (node->getParent()->getChildren().size() > 0 && node->getParent()->getChildren()[0]->getType() == NodeType::type) {
		auto idNode = nextAtom(node);
		auto label = idNode->getLabel();
		auto table = node->getSymbolTable();
		auto entry = std::make_shared<SymbolTableEntry>();
		table->addEntry(label, entry, idNode);
		node->addChild(idNode);
	} else
		node->addChild(nextAtom(node));

	return node;
}

std::shared_ptr<DerivationNode> Parser::semicolon(const std::weak_ptr<DerivationNode> &parent) {
	auto node = std::make_shared<DerivationNode>(NodeType::semicolon, parent);

	if (atom != ";")
		throw std::domain_error(" Semicolon expected at line: " + currentLine());

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
