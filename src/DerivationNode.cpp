#include "DerivationNode.hpp"

#include <iostream>

DerivationNode::DerivationNode(NodeType type)
	: type(type), depth(0), visited(false), parent(std::weak_ptr<DerivationNode>())
{
}

DerivationNode::DerivationNode(NodeType type, const std::weak_ptr<DerivationNode> &parent)
	: type(type), visited(false), parent(parent)
{
	auto p = parent.lock();
	depth = p ? p->getDepth() + 1 : 0;
}

NodeType DerivationNode::getType()
{
	return type;
}

std::string DerivationNode::getLabel() const
{
	return label;
}

void DerivationNode::setLabel(const std::string &label)
{
	this->label = label;
}

std::shared_ptr<DerivationNode> DerivationNode::getParent() const
{
	return parent.lock();
}

std::vector<std::shared_ptr<DerivationNode>> DerivationNode::getChildren() const
{
	return children;
}

void DerivationNode::removeChild(size_t index)
{
	if (index > children.size() - 1)
		return;

	size_t counter = 0;

	for (auto it = children.begin(); it != children.end(); ++it) {
		if (counter++ == index) {
			children.erase(it);
			break;
		}
	}
}

size_t DerivationNode::getDepth() const
{
	return depth;
}

bool DerivationNode::wasVisited()
{
	return visited;
}

void DerivationNode::setVisited()
{
	visited = true;
}

void DerivationNode::addChild(const std::shared_ptr<DerivationNode> &child)
{
	children.push_back(child);
}

std::shared_ptr<SymbolTable> DerivationNode::createScope()
{
	localScope = std::make_shared<SymbolTable>();
	return localScope;
}

std::shared_ptr<SymbolTable> DerivationNode::getSymbolTable()
{
	if (localScope)
		return localScope;

	auto p = parent.lock();
	return p->getSymbolTable();
}

std::shared_ptr<SymbolTableEntry> DerivationNode::getSymbolEntry(const std::string &symbol)
{
	std::shared_ptr<SymbolTableEntry> result;
	if (localScope)
		result = localScope->getEntry(symbol);
	if (result)
		return result;

	auto p = parent.lock();

	while (result == nullptr && p != nullptr) {
		auto table = p->getSymbolTable();
		result = table->getEntry(symbol);
		p = p->getParent();
	}

	return result;
}

size_t DerivationNode::findIndexInParent()
{
	auto children = getParent()->getChildren();

	for (size_t i = 0; i < children.size(); ++i)
		if (children[i].get() == this)
			return i;

	return 0;
}

std::string DerivationNode::nodeTypeToString(NodeType type)
{
	switch (type) {
	case NodeType::program:
		return "program";
	case NodeType::funDef:
		return "funDef";
	case NodeType::funDecls:
		return "funDecls";
	case NodeType::funDecl:
		return "funDecl";
	case NodeType::args:
		return "args";
	case NodeType::stmts:
		return "stmts";
	case NodeType::stmt:
		return "stmt";
	case NodeType::condStmt:
		return "condStmt";
	case NodeType::whileStmt:
		return "whileStmt";
	case NodeType::forStmt:
		return "forStmt";
	case NodeType::callStmt:
		return "callStmt";
	case NodeType::indexStmt:
		return "indexStmt";
	case NodeType::varDef:
		return "varDef";
	case NodeType::varDecl:
		return "varDecl";
	case NodeType::listStmt:
		return "listStmt";
	case NodeType::basicListIter:
		return "basicListIter";
	case NodeType::retStmt:
		return "retStmt";
	case NodeType::listIfStmt:
		return "listIfStmt";
	case NodeType::assignStmt:
		return "assignStmt";
	case NodeType::ifStmt:
		return "ifStmt";
	case NodeType::elseStmt:
		return "elseStmt";
	case NodeType::printStmt:
		return "printStmt";
	case NodeType::expr:
		return "expr";
	case NodeType::andExpr:
		return "andExpr";
	case NodeType::compExpr:
		return "compExpr";
	case NodeType::sumExpr:
		return "sumExpr";
	case NodeType::mulExpr:
		return "mulExpr";
	case NodeType::val:
		return "val";
	case NodeType::type:
		return "type";
	case NodeType::identifier:
		return "identifier";
	case NodeType::semicolon:
		return "semicolon";
	case NodeType::symbol:
		return "symbol";
	}

	return "invalid state";
}
