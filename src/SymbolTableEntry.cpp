#include "SymbolTableEntry.hpp"

SymbolTableEntry::SymbolTableEntry()
	: funcArgsNo(0), listSize(0)
{}

bool SymbolTableEntry::isFunction()
{
	return type == Type::VOID_FUNCTION || type == Type::INT_FUNCTION || type == Type::FLOAT_FUNCTION || type == Type::BOOL_FUNCTION;
}

void SymbolTableEntry::setIdentifier(const std::string &identifier)
{
	this->identifier = identifier;
}

std::string SymbolTableEntry::getIdentifier()
{
	return identifier;
}

void SymbolTableEntry::setType(Type type)
{
	this->type = type;
}

Type SymbolTableEntry::getType()
{
	return type;
}

void SymbolTableEntry::setValue(const std::shared_ptr<void> &value)
{
	this->value = value;
}

std::shared_ptr<void> SymbolTableEntry::getValue()
{
	return value;
}

void SymbolTableEntry::setFuncArgsNo(size_t number)
{
	funcArgsNo = number;
}

size_t SymbolTableEntry::getFuncArgsNo()
{
	return funcArgsNo;
}

void SymbolTableEntry::setListSize(size_t number)
{
	listSize = number;
}

size_t SymbolTableEntry::getListSize()
{
	return listSize;
}

std::string SymbolTableEntry::typeToString(Type type)
{
	switch (type) {
	case Type::INT:
		return "INT";

	case Type::FLOAT:
		return "FLOAT";

	case Type::BOOL:
		return "BOOL";

	case Type::INT_LIST:
		return "INT_LIST";

	case Type::FLOAT_LIST:
		return "FLOAT_LIST";

	case Type::BOOL_LIST:
		return "BOOL_LIST";

	case Type::VOID_FUNCTION:
		return "VOID_FUNCTION";

	case Type::FLOAT_FUNCTION:
		return "FLOAT_FUNCTION";

	case Type::INT_FUNCTION:
		return "INT_FUNCTION";

	case Type::BOOL_FUNCTION:
		return "BOOL_FUNCTION";
	}

	return "NONE";
}
