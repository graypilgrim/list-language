#include "SymbolTableEntry.hpp"

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

void SymbolTableEntry::setTableType(Type type)
{
	tableType = type;
}

Type SymbolTableEntry::getTableType()
{
	return tableType;
}

void SymbolTableEntry::setListType(Type type)
{
	listType = type;
}

Type SymbolTableEntry::getListType()
{
	return listType;
}

void SymbolTableEntry::setFuncType(Type type)
{
	funcType = type;
}

Type SymbolTableEntry::getFuncType()
{
	return funcType;
}

void SymbolTableEntry::setFuncArgsNo(size_t number)
{
	funcArgsNo = number;
}

size_t SymbolTableEntry::getFuncArgsNo()
{
	return funcArgsNo;
}
