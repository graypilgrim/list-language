#ifndef SYMBOL_TABLE_ENTRY_HPP
#define SYMBOL_TABLE_ENTRY_HPP

#include <memory>
#include <string>

enum class Type
{
	VOID,
	INT,
	FLOAT,
	BOOL,
	TABLE,
	LIST,
	FUNCTION
};

class SymbolTableEntry
{
public:
	SymbolTableEntry(const std::string &name);

	void setType(Type type);
	Type getType();

	void setValue(const std::shared_ptr<void> &value);
	std::shared_ptr<void> getValue();

	void setTableType(Type type);
	Type getTableType();

	void setListType(Type type);
	Type getListType();

	void setFuncType(Type type);
	Type getFuncType();

	void setFuncArgsNo(size_t number);
	size_t getFuncArgsNo();

private:
	std::string name;
	Type type;
	std::shared_ptr<void> value;

	Type tableType;
	Type listType;

	Type funcType;
	size_t funcArgsNo;
};

#endif
