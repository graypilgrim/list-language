#include "Interpreter.hpp"

#include <iostream>

Interpreter::Interpreter(const std::shared_ptr<DerivationTree> &tree)
	:tree(tree)
{
	if (!tree)
		throw std::runtime_error("No tree provided");
}

void Interpreter::execute()
{
	fillSymbolTables();
	current = findMain();
	if (!current)
		throw std::runtime_error("No main function");

	while (current) {
		processStmt();
		nextNode();
	}
}

void Interpreter::fillSymbolTables()
{
	for (auto &i : tree->getTables()) {
		for (auto &tuple : i->getScope()) {
			auto node = tuple.second.second.lock();
			auto entry = tuple.second.first;
			setType(node, entry);

			if (entry->isFunction())
				setArgsNo(node, entry);
		}
	}
}

void Interpreter::printSymbolTables()
{
	std::cout << "Symbol tables" << std::endl;
	size_t counter = 0;

	for (auto &i : tree->getTables()) {
		std::cout << "  Table" << counter++ << ": " << std::endl;
		for (auto &tuple : i->getScope()) {
			auto entry = tuple.second.first;

			std::cout << "\t"<< tuple.first
				<< " " << SymbolTableEntry::typeToString(entry->getType())
				<< " " << entry->getFuncArgsNo()
				<< " " << entry->getListSize();

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
			if (entry->getType() == Type::INT_LIST) {
				std::cout << " [ ";
				auto array = (int*)(entry->getValue().get());
				for (size_t i = 0; i < entry->getListSize(); ++i)
					std::cout << array[i] << " ";

				std::cout << "]";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

void Interpreter::setType(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry)
{
	bool list = false;
	bool function = false;
	auto grandParent = node->getParent()->getParent();

	if (grandParent->getType() == NodeType::funDecl)
		function = true;

	auto typeNode = grandParent->getChildren()[0]->getChildren()[0];
	if (typeNode->getLabel() == "list") {
		list = true;
		typeNode = typeNode->getParent()->getChildren()[2]->getChildren()[0];
	}

	entry->setType(deduceType(list, function, typeNode->getLabel()));
}

void Interpreter::setArgsNo(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry)
{
	size_t counter = 0;

	auto argNode = node->getParent()->getParent();

	for (size_t i = 0; i < argNode->getChildren().size(); ++i) {
		if (argNode->getChildren()[i]->getType() == NodeType::args) {
			argNode = argNode->getChildren()[i];
			i = 0;
			++counter;
		}
	}

	entry->setFuncArgsNo(counter);
}

Type Interpreter::deduceType(bool isList, bool isFunction, const std::string &type)
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

std::shared_ptr<DerivationNode> Interpreter::findMain()
{
	for (auto &i : tree->getTables()) {
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

void Interpreter::nextNode()
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

void Interpreter::assignValue(const std::shared_ptr<DerivationNode> &n)
{
	auto node = n ? n : current;

	auto indexInParent = node->findIndexInParent();
	auto parent = node->getParent();

	if (parent->getType() == NodeType::varDecl || parent->getType() == NodeType::stmt) {
		auto lValNode = parent->getChildren()[indexInParent-2]->getChildren()[0];
		auto lVal = lValNode->getLabel();
		auto lValEntry = lValNode->getSymbolEntry(lVal);

		auto rValNode = node->getChildren()[0];
		auto rVal = rValNode->getLabel();
		lValEntry->setValue(evaluate(rValNode));
	} else if (parent->getType() == NodeType::indexStmt) {
		auto index = evaluate(parent->getChildren()[0]);
		auto indexNumber = *(int*)(index.get());

		auto lValNode = parent->getParent()->getChildren()[0]->getChildren()[0];
		auto lValEntry = lValNode->getSymbolEntry(lValNode->getLabel());
		auto lValValue = (int*)(lValEntry->getValue().get());

		auto rValNode = node->getChildren()[0];
		auto rValResult = evaluate(rValNode);
		lValValue[indexNumber] = *(int*)(rValResult.get());
	}

}

void Interpreter::processStmt()
{
	auto type = current->getType();

	if (type == NodeType::varDef || type == NodeType::assignStmt)
		assignValue();
	else if (type == NodeType::condStmt)
		processIf();
	else if (type == NodeType::forStmt)
		processFor();
	else if (type == NodeType::listStmt)
		processListStmt();
	else if (type == NodeType::indexStmt)
		processIndexStmt();
	else if (type == NodeType::printStmt)
		processPrintStmt();
}

void Interpreter::processIf()
{
	auto condNode = current->getChildren()[0]->getChildren()[1];
	auto endSign = current->getChildren()[1]->getChildren().size() == 1 ? ";" : "}";

	auto condition = evaluate(condNode);
	if (*(bool*)(condition.get())) {
		current = current->getChildren()[1];
	} else {
		while (current->getLabel() != endSign)
			nextNode();
	}
}

void Interpreter::processFor()
{
	auto forNode = current;
	auto loopBodyNode = current->getChildren()[6];
	auto endSign = loopBodyNode->getChildren().size() == 1 ? ";" : "}";

	while (current->getType() != NodeType::assignStmt)
		nextNode();
	assignValue();

	while (current->getLabel() != ";")
		nextNode();
	nextNode();

	auto condNode = current;

	while (current->getType() != NodeType::assignStmt)
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

void Interpreter::processListStmt()
{
	auto table = current->createScope();
	tree->pushTable(table);
	auto entry = std::make_shared<SymbolTableEntry>();
	entry->setValue(std::shared_ptr<void>(new int));
	auto idNode = current->getChildren()[0]->getChildren()[2]->getChildren()[0];
	auto id = idNode->getLabel();
	table->addEntry(id, entry, idNode);

	auto listName = current->getChildren()[0]->getChildren()[4]->getChildren()[0]->getLabel();
	auto listEntry = current->getSymbolEntry(listName);

	if (listEntry->getType() == Type::INT_LIST)
		entry->setType(Type::INT);
	else if (listEntry->getType() == Type::BOOL_LIST)
		entry->setType(Type::BOOL);
	else if (listEntry->getType() == Type::FLOAT_LIST)
		entry->setType(Type::FLOAT);

	auto listVal = (int*)(listEntry->getValue().get());
	for (size_t i = 0; i < listEntry->getListSize(); ++i) {
		auto itVal = entry->getValue();
		*(int*)(itVal.get()) = listVal[i];
		auto newVal = evaluate(current->getChildren()[0]->getChildren()[0]);
		listVal[i] = *(int*)(newVal.get());
	}
}

void Interpreter::processIndexStmt()
{
	if (current->getParent()->getType() == NodeType::varDecl) {
		auto siblings = current->getParent()->getChildren();
		std::string id;
		for (auto &n : siblings)
			if (n->getType() == NodeType::identifier) {
				id = n->getChildren()[0]->getLabel();
				break;
			}

		auto entry = current->getSymbolEntry(id);
		auto listSize = evaluate(current->getChildren()[0]);
		auto listSizeNumber = *(int*)(listSize.get());
		entry->setListSize(listSizeNumber);
		auto val = std::shared_ptr<void>(new int[listSizeNumber]);
		entry->setValue(val);
	}
}

void Interpreter::processPrintStmt()
{
	if (current->getChildren()[0]->getType() == NodeType::symbol) {
		std::cout << current->getChildren()[0]->getLabel();
	} else {
		auto result = evaluate(current->getChildren()[0]);
		std::cout << *(int*)(result.get());
	}

	if (current->getParent()->getChildren()[0]->getLabel() == "println")
		std::cout << std::endl;

	while (current->getLabel() != ";")
		nextNode();
}

std::shared_ptr<void> Interpreter::evaluate(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getType() == NodeType::val) {
		if (node->getChildren()[0]->getType() == NodeType::identifier) {
			auto id = node->getChildren()[0]->getChildren()[0];
			auto entry = id->getSymbolEntry(id->getLabel());

			if (node->getChildren().size() > 1) {
				auto index = evaluate(node->getChildren()[2]->getChildren()[0]);
				auto indexNumber = *(int*)(index.get());
				auto val = (int*)(entry->getValue().get());

				return std::shared_ptr<void>(new int(val[indexNumber]));
			}

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

	if (node->getType() == NodeType::mulExpr) {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			if (node->getChildren()[i]->getLabel() == "*")
				*(int*)(result.get()) *= *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == "/")
				*(int*)(result.get()) /= *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	if (node->getType() == NodeType::sumExpr) {
		std::shared_ptr<void> result = evaluate(node->getChildren()[0]);

		for (size_t i = 1; i < node->getChildren().size(); i+=2) {
			if (node->getChildren()[i]->getLabel() == "+")
				*(int*)(result.get()) += *(int*)(evaluate(node->getChildren()[i+1]).get());
			if (node->getChildren()[i]->getLabel() == "-")
				*(int*)(result.get()) -= *(int*)(evaluate(node->getChildren()[i+1]).get());
		}

		return result;
	}

	if (node->getType() == NodeType::compExpr) {
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

	if (node->getType() == NodeType::andExpr) {
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
