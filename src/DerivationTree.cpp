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
	current = findMain();
	if (!current)
		throw std::runtime_error("No main function");

	while (current) {
		processStmt();
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
			std::cout << "-";
		std::cout << "|";
		std::cout << node->getLabel() << std::endl;

		auto children = node->getChildren();

		for (auto n = children.rbegin(); n != children.rend(); ++n)
			stack.push_back(*n);
	}
}

void DerivationTree::printSymbolTables()
{
	std::cout << "Symbol tables" << std::endl;
	size_t counter = 0;

	for (auto &i : symbolTables) {
		std::cout << "  Table" << counter++ << ": " << std::endl;
		for (auto &tuple : i->getScope()) {
			auto entry = tuple.second.first;

			std::cout << "\t"<< tuple.first
				<< " " << SymbolTableEntry::typeToString(entry->getType())
				<< " " << entry->getFuncArgsNo();

			if (entry->getValue() == nullptr) {
				std::cout << std::endl;
				continue;
			}

			if (entry->getType() == Type::INT) {
				std::cout << " " << *(int*)(entry->getValue().get());
			}
			if (entry->getType() == Type::FLOAT) {
				std::cout << " " << *(float*)(entry->getValue().get());
			}
			if (entry->getType() == Type::BOOL) {
				std::cout << " " << *(bool*)(entry->getValue().get());
			}
			std::cout << std::endl;
		}
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

void DerivationTree::assignValue(const std::shared_ptr<DerivationNode> &n)
{
	auto node = n ? n : current;

	auto indexInParent = node->findIndexInParent();
	auto parent = node->getParent();

	auto lValNode = parent->getChildren()[indexInParent-2]->getChildren()[0];
	auto lVal = lValNode->getLabel();
	auto lValEntry = lValNode->getSymbolTable()->getEntry(lVal);

	auto rValNode = node->getChildren()[0];
	auto rVal = rValNode->getLabel();
	lValEntry->setValue(evaluate(rValNode));
}

void DerivationTree::processStmt()
{
	if (current->getLabel() == "varDef" || current->getLabel() == "assignStmt")
		assignValue();
	else if (current->getLabel() == "condStmt")
		processIf();
	else if (current->getLabel() == "forStmt")
		processFor();
}

void DerivationTree::processIf()
{
	auto condNode = current->getChildren()[0]->getChildren()[1];
	auto condition = evaluate(condNode);
	if (*(bool*)(condition.get())) {
		current = current->getChildren()[1];
	} else {
		current->removeChild(1);
		dfsStack.pop_back();
		dfsStack.pop_back();
	}
}

void DerivationTree::processFor()
{
	auto forNode = current;
	auto loopBodyNode = current->getChildren()[6];
	auto endSign = loopBodyNode->getChildren().size() == 1 ? ";" : "}";

	while (current->getLabel() != "assignStmt")
		nextNode();
	assignValue();

	while (current->getLabel() != ";")
		nextNode();
	nextNode();

	auto condNode = current;

	while (current->getLabel() != "assignStmt")
		nextNode();

	auto iteratorChangeNode = current;

	while (current->getLabel() != ")")
		nextNode();
	dfsStack.pop_back();

	while (*(bool*)(evaluate(condNode).get())) {
		dfsStack.push_back(loopBodyNode);
		nextNode();
		while (current->getLabel() != endSign) {
			processStmt();
			nextNode();
		}
		assignValue(iteratorChangeNode);
	}
}

std::shared_ptr<void> DerivationTree::evaluate(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getLabel() == "val") {
		if (node->getChildren()[0]->getLabel() == "identifier") {
			auto id = node->getChildren()[0]->getChildren()[0];
			auto entry = id->getSymbolTable()->getEntry(id->getLabel());
			auto val = entry->getValue();
			return std::shared_ptr<void>(new int(*(int*)(val.get())));
		} else {
			auto val = node->getChildren()[0]->getLabel();
			if (val == "true")
				val = "1";
			else if (val == "false")
				val = "0";
			return std::shared_ptr<void>(new int(std::stoi(val)));
		}
	}

	if (node->getLabel() == "mulExpr") {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			if (node->getChildren()[i]->getLabel() == "*")
				*(int*)(result.get()) *= *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == "/")
				*(int*)(result.get()) /= *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	if (node->getLabel() == "sumExpr") {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			if (node->getChildren()[i]->getLabel() == "+")
				*(int*)(result.get()) += *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == "-")
				*(int*)(result.get()) -= *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	if (node->getLabel() == "comExpr") {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			if (node->getChildren()[i]->getLabel() == "<")
				*(int*)(result.get()) = *(int*)(result.get()) < *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == ">")
				*(int*)(result.get()) = *(int*)(result.get()) > *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == "==")
				*(int*)(result.get()) = *(int*)(result.get()) == *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	if (node->getLabel() == "andExpr") {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
				*(int*)(result.get()) = *(int*)(result.get()) && *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

	for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			*(int*)(result.get()) = *(int*)(result.get()) || *(int*)(evaluate(node->getChildren()[i+1]).get());
	}

	return result;
}
