#include "SymbolTableEntry.hpp"

SymbolTableEntry::SymbolTableEntry()
	: funcArgsNo(0), listSize(0)
{}

bool SymbolTableEntry::isFunction()
{
	return function;
}

bool SymbolTableEntry::isList()
{
	return list;
}

void SymbolTableEntry::setValue(const std::shared_ptr<Value> &value)
{
	this->value = value;
}

std::shared_ptr<Value> SymbolTableEntry::getValue()
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
