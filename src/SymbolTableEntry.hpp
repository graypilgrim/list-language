#ifndef SYMBOL_TABLE_ENTRY_HPP
#define SYMBOL_TABLE_ENTRY_HPP

#include "Value.hpp"

#include <memory>
#include <string>

class SymbolTableEntry
{
public:
	SymbolTableEntry();

	bool isFunction();
	void setFunction(bool flag);
	bool isList();
	void setList(bool flag);

	void setValue(const std::shared_ptr<Value> &value);
	std::shared_ptr<Value> getValue();

	void setFuncArgsNo(size_t number);
	size_t getFuncArgsNo();

	void setListSize(size_t number);
	size_t getListSize();

	static std::string typeToString(Type type);

private:
	bool function;
	bool list;
	std::shared_ptr<Value> value;
	size_t funcArgsNo;
	size_t listSize;
};

#endif
