#include "DerivationTree.hpp"

#include <iostream>

DerivationTree::DerivationTree(const std::shared_ptr<DerivationNode> &root)
	: root(root)
{}

std::shared_ptr<DerivationNode> DerivationTree::getCurrent()
{
	return current;
}

void DerivationTree::pushTable(const std::shared_ptr<SymbolTable> &table)
{
	symbolTables.emplace_back(table);
}

void DerivationTree::execute()
{
	std::cout << ">>>>DEBUG: " << __FUNCTION__ << ": execution" << std::endl;
	current = findMain();
	if (!current)
		throw std::runtime_error("No main function");

	while (current) {
		std::cout << ">>>>DEBUG: " << __FUNCTION__ << ": " << current->getLabel() << std::endl;
		nextNode();
	}
}

void DerivationTree::fillSymbolTables()
{
	for (auto &i : symbolTables) {
		for (auto &tuple : i->getScope()) {
			auto node = tuple.second.second.lock();
			auto entry = tuple.second.first;
			setType(node, entry);

			if (entry->isFunction())
				setArgsNo(node, entry);
		}
	}
}

void DerivationTree::printTree()
{
	std::vector<std::shared_ptr<DerivationNode>> stack;
	stack.push_back(root);

	while (!stack.empty()) {
		auto node = stack.back();
		stack.pop_back();

		for (size_t i = 0 ; i < node->getDepth(); ++i)
			std::cout << "--";
		std::cout << "|";
		std::cout << node->getLabel() << std::endl;

		auto children = node->getChildren();

		for (auto n = children.rbegin(); n != children.rend(); ++n)
			stack.push_back(*n);
	}
}

void DerivationTree::printSymbolTables()
{
	for (auto &i : symbolTables) {
		std::cout << "Table: " << std::endl;
		for (auto &tuple : i->getScope())
			std::cout << "\t"<< tuple.first
				<< " " << SymbolTableEntry::typeToString(tuple.second.first->getType())
				<< " " << tuple.second.first->getFuncArgsNo() << std::endl;
		std::cout << std::endl;
	}
}

void DerivationTree::setType(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry)
{
	bool list = false;
	bool function = false;
	auto grandParent = node->getParent()->getParent();

	if (grandParent->getLabel() == "funDecl")
		function = true;

	auto typeNode = grandParent->getChildren()[0]->getChildren()[0];
	if (typeNode->getLabel() == "list") {
		list = true;
		typeNode = typeNode->getParent()->getChildren()[2]->getChildren()[0];
	}

	entry->setType(deduceType(list, function, typeNode->getLabel()));
}

void DerivationTree::setArgsNo(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry)
{
	size_t counter = 0;

	auto argNode = node->getParent()->getParent();

	for (size_t i = 0; i < argNode->getChildren().size(); ++i) {
		if (argNode->getChildren()[i]->getLabel() == "args") {
			argNode = argNode->getChildren()[i];
			i = 0;
			++counter;
		}
	}

	entry->setFuncArgsNo(counter);
}

Type DerivationTree::deduceType(bool isList, bool isFunction, const std::string &type)
{
	if (type == "int" && isList)
		return Type::INT_LIST;

	if (type == "float" && isList)
		return Type::FLOAT_LIST;

	if (type == "bool" && isList)
		return Type::BOOL_LIST;

	if (type == "int" && isFunction)
		return Type::INT_FUNCTION;

	if (type == "float" && isFunction)
		return Type::FLOAT_FUNCTION;

	if (type == "bool" && isFunction)
		return Type::BOOL_FUNCTION;

	if (type == "void" && isFunction)
		return Type::VOID_FUNCTION;

	if (type == "int")
		return Type::INT;

	if (type == "float")
		return Type::FLOAT;

	if (type == "bool")
		return Type::BOOL;

	return Type::FLOAT;
}

std::shared_ptr<DerivationNode> DerivationTree::findMain()
{
	for (auto &i : symbolTables) {
		for (auto &tuple : i->getScope()) {
			auto node = tuple.second.second.lock();
			if (node->getLabel() == "main") {
				auto grandParent = node->getParent()->getParent();
				auto funDef = grandParent->getChildren().back();
				dfsStack.push_back(funDef);
				return node;
			}
		}
	}

	return nullptr;
}

void DerivationTree::nextNode()
{
	if (dfsStack.empty()) {
		current = nullptr;
		return;
	}

	auto node = dfsStack.back();
	dfsStack.pop_back();

	auto children = node->getChildren();

	for (auto n = children.rbegin(); n != children.rend(); ++n)
		dfsStack.push_back(*n);

	current = node;
}
