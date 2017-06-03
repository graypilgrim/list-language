#include "SymbolTableEntry.hpp"

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
