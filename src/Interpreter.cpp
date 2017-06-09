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

void Interpreter::printList(const std::shared_ptr<SymbolTableEntry> &entry)
{
	std::cout << " [ ";
	switch (entry->getValue()->getType()) {
	case Type::INT: {
		auto array = entry->getValue()->getInt().get();
		for (size_t i = 0; i < entry->getListSize(); ++i)
			std::cout << array[i] << " ";
		break;
	}

	case Type::FLOAT: {
			auto array = entry->getValue()->getFloat().get();
			for (size_t i = 0; i < entry->getListSize(); ++i)
				std::cout << array[i] << " ";
			break;
	}

	case Type::BOOL: {
		auto array = entry->getValue()->getBool().get();
		for (size_t i = 0; i < entry->getListSize(); ++i)
			std::cout << array[i] << " ";

		break;
	}
	}

	std::cout << "]";
}

void Interpreter::printSymbolTables()
{
	std::cout << "Symbol tables" << std::endl;
	size_t counter = 0;

	for (auto &i : tree->getTables()) {
		std::cout << "  Table" << counter++ << ": " << std::endl;
		for (auto &tuple : i->getScope()) {
			auto entry = tuple.second.first;

			auto val = entry->getValue();

			std::cout << "\t" << tuple.first
				<< " " << val->typeToString()
				<< " " << entry->getFuncArgsNo()
				<< " " << entry->getListSize();

			if (entry->isList()) {
				printList(entry);
				std::cout << std::endl;
			} else {
				if (val->getType() == Type::INT)
					std::cout << " " << (val->getInt() ? *(val->getInt()) : 0);

				if (val->getType() == Type::FLOAT)
					std::cout << " " << (val->getFloat() ? *(val->getFloat()) : 0.0f);

				if (val->getType() == Type::BOOL)
					std::cout << " " << (val->getBool() ? *(val->getBool()) : false);

				std::cout << std::endl;
			}
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
		auto listSizeNode = grandParent->getChildren()[3];


		if (listSizeNode->getLabel() == "]")
			throw std::runtime_error("List has no size");

		auto size = evaluate(listSizeNode->getChildren()[0]);
		auto val = std::shared_ptr<int>(new int[*(size->getInt())]);
		entry->getValue()->setInt(val);
		entry->setListSize(*(size->getInt()));
	}

	entry->setFunction(function);
	entry->setList(list);
	entry->getValue()->setType(deduceType(typeNode->getLabel()));
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

Type Interpreter::deduceType(const std::string &type)
{
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
		auto indexNumber = *(index->getInt());

		auto lValNode = parent->getParent()->getChildren()[0]->getChildren()[0];
		auto lValEntry = lValNode->getSymbolEntry(lValNode->getLabel());

		auto rValNode = node->getChildren()[0];
		auto rValResult = evaluate(rValNode);

		switch (lValEntry->getValue()->getType()) {
		case Type::INT: {
			auto lValValue = lValEntry->getValue()->getInt().get();
			lValValue[indexNumber] = *(rValResult->getInt());
			break;
		}
		case Type::FLOAT: {
			auto lValValue = lValEntry->getValue()->getFloat().get();
			lValValue[indexNumber] = *(rValResult->getFloat());
			break;
		}
		case Type::BOOL: {
			auto lValValue = lValEntry->getValue()->getBool().get();
			lValValue[indexNumber] = *(rValResult->getBool());
			break;
		}
		}
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
	else if (type == NodeType::printStmt)
		processPrintStmt();
}

void Interpreter::processIf()
{
	auto condNode = current->getChildren()[0]->getChildren()[1];
	auto endSign = current->getChildren()[1]->getChildren().size() == 1 ? ";" : "}";

	auto condition = evaluate(condNode);
	if (*(condition->getBool())) {
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

	while (*(evaluate(condNode)->getBool())) {
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
	entry->setValue(std::make_shared<Value>(Type::INT));
	entry->getValue()->setInt(std::make_shared<int>(0));
	auto idNode = current->getChildren()[0]->getChildren()[2]->getChildren()[0];
	auto id = idNode->getLabel();
	table->addEntry(id, entry, idNode);

	auto listName = current->getChildren()[0]->getChildren()[4]->getChildren()[0]->getLabel();
	auto listEntry = current->getSymbolEntry(listName);
	auto type = listEntry->getValue()->getType();

	switch (type) {
	case Type::INT: {
		auto listVal = listEntry->getValue()->getInt().get();
		for (size_t i = 0; i < listEntry->getListSize(); ++i) {
			auto itVal = entry->getValue()->getInt().get();
			*(itVal) = listVal[i];

			auto newVal = evaluate(current->getChildren()[0]->getChildren()[0]);
			listVal[i] = *(newVal->getInt().get());
		}
		break;
	}

	case Type::FLOAT: {
		auto listVal = listEntry->getValue()->getFloat().get();
		for (size_t i = 0; i < listEntry->getListSize(); ++i) {
			auto itVal = entry->getValue()->getFloat().get();
			*(itVal) = listVal[i];
			auto newVal = evaluate(current->getChildren()[0]->getChildren()[0]);
			listVal[i] = *(newVal->getFloat().get());
		}
		break;
	}

	case Type::BOOL: {
		auto listVal = listEntry->getValue()->getBool().get();
		for (size_t i = 0; i < listEntry->getListSize(); ++i) {
			auto itVal = entry->getValue()->getBool().get();
			*(itVal) = listVal[i];
			auto newVal = evaluate(current->getChildren()[0]->getChildren()[0]);
			listVal[i] = *(newVal->getBool().get());
		}
		break;
	}
	}
}

void Interpreter::processPrintStmt()
{
	if (current->getChildren()[0]->getType() == NodeType::symbol) {
		std::cout << current->getChildren()[0]->getLabel();
	} else {
		auto result = evaluateVal(current->getChildren()[0]);

		switch (result->getType()) {
		case Type::INT:
			std::cout << *(result->getInt());
			break;

		case Type::FLOAT:
			std::cout << *(result->getFloat());
			break;

		case Type::BOOL:
			std::cout << *(result->getBool());
			break;
		}
	}

	if (current->getParent()->getChildren()[0]->getLabel() == "println")
		std::cout << std::endl;

	while (current->getLabel() != ";")
		nextNode();
}

std::shared_ptr<Value> Interpreter::evaluate(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getChildren().size() == 1)
		return evaluateAnd(node->getChildren()[0]);

	auto result = std::make_shared<Value>(Type::BOOL);

	for (size_t i = 1; i < node->getChildren().size(); i+=2) {
		auto nextItem = evaluateComp(node->getChildren()[i+1]);

		result->setBool(std::make_shared<bool>(*result || *nextItem));
	}

	return result;
}

std::shared_ptr<Value> Interpreter::evaluateVal(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getChildren()[0]->getType() == NodeType::identifier) {
		auto id = node->getChildren()[0]->getChildren()[0];

		auto entry = id->getSymbolEntry(id->getLabel());

		if (node->getChildren().size() == 1) {
			auto result = std::make_shared<Value>(entry->getValue()->getType());

			switch (entry->getValue()->getType()) {
			case Type::INT: {
				auto val = std::make_shared<int>(*(entry->getValue()->getInt()));
				result->setInt(val);
				break;
			}

			case Type::FLOAT: {
				auto val = std::make_shared<float>(*(entry->getValue()->getFloat()));
				result->setFloat(val);
				break;
			}

			case Type::BOOL: {
				auto val = std::make_shared<bool>(*(entry->getValue()->getBool()));
				result->setBool(val);
				break;
			}
			}
			return result;
		}

		auto index = evaluate(node->getChildren()[2]->getChildren()[0]);
		if (index->getType() != Type::INT)
			throw std::runtime_error("Invalid index type");

		auto indexNumber = *(index->getInt().get());
		auto valType = entry->getValue()->getType();
		auto result = std::make_shared<Value>(valType);

		switch (valType) {
		case Type::INT: {
			auto intVal = entry->getValue()->getInt().get();
			result->setInt(std::make_shared<int>(intVal[indexNumber]));
			return result;
		}

		case Type::FLOAT: {
			auto floatVal = entry->getValue()->getFloat().get();
			result->setFloat(std::make_shared<float>(floatVal[indexNumber]));
			return result;
		}

		case Type::BOOL: {
			auto boolVal = entry->getValue()->getBool().get();
			result->setBool(std::make_shared<bool>(boolVal[indexNumber]));
			return result;
		}
		}

	} else {
		auto val = node->getChildren()[0]->getLabel();

		try
		{
			auto number = stoi(val);
			auto result = std::make_shared<Value>(Type::INT);
			result->setInt(std::make_shared<int>(number));
			return result;
		}
		catch (...)
		{}

		try
		{
			auto number = stof(val);
			auto result = std::make_shared<Value>(Type::FLOAT);
			result->setFloat(std::make_shared<float>(number));
			return result;
		}
		catch (...)
		{}

		auto result = std::make_shared<Value>(Type::BOOL);

		if (val == "true")
			result->setBool(std::make_shared<bool>(true));
		else if (val == "false")
			result->setBool(std::make_shared<bool>(false));

		return result;
	}

	return std::make_shared<Value>(Type::INT);
}

std::shared_ptr<Value> Interpreter::evaluateMul(const std::shared_ptr<DerivationNode> &node)
{
	auto result = evaluateVal(node->getChildren()[0]);


	for (size_t i = 1; i < node->getChildren().size(); i+=2) {
		auto nextItem = evaluateVal(node->getChildren()[i+1]);

		if (node->getChildren()[i]->getLabel() == "*")
			*result *= *nextItem;
		if (node->getChildren()[i]->getLabel() == "/")
			*result /= *nextItem;
	}

	return result;
}

std::shared_ptr<Value> Interpreter::evaluateSum(const std::shared_ptr<DerivationNode> &node)
{
	auto result = evaluateMul(node->getChildren()[0]);

	for (size_t i = 1; i < node->getChildren().size(); i+=2) {
		auto nextItem = evaluateMul(node->getChildren()[i+1]);

		if (node->getChildren()[i]->getLabel() == "+")
			*result += *nextItem;
		if (node->getChildren()[i]->getLabel() == "-")
			*result -= *nextItem;
	}

	return result;
}

std::shared_ptr<Value> Interpreter::evaluateComp(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getChildren().size() == 1)
		return evaluateSum(node->getChildren()[0]);

	auto result = std::make_shared<Value>(Type::BOOL);

	auto left = evaluateSum(node->getChildren()[0]);
	auto right = evaluateSum(node->getChildren()[2]);


	if (node->getChildren()[1]->getLabel() == "<")
		result->setBool(std::make_shared<bool>(*left < *right));
	if (node->getChildren()[1]->getLabel() == ">")
		result->setBool(std::make_shared<bool>(*left > *right));
	if (node->getChildren()[1]->getLabel() == "==")
		result->setBool(std::make_shared<bool>(*left == *right));
	if (node->getChildren()[1]->getLabel() == "!=")
		result->setBool(std::make_shared<bool>(*left == *right));


	return result;
}

std::shared_ptr<Value> Interpreter::evaluateAnd(const std::shared_ptr<DerivationNode> &node)
{
	if (node->getChildren().size() == 1)
		return evaluateComp(node->getChildren()[0]);

	auto result = std::make_shared<Value>(Type::BOOL);

	for (size_t i = 1; i < node->getChildren().size(); i+=2) {
		auto nextItem = evaluateComp(node->getChildren()[i+1]);

		result->setBool(std::make_shared<bool>(*result && *nextItem));
	}

	return result;
}
