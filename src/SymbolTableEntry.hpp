#ifndef SYMBOL_TABLE_ENTRY_HPP
#define SYMBOL_TABLE_ENTRY_HPP

#include <memory>
#include <string>

enum class Type
{
	INT,
	FLOAT,
	BOOL,
	INT_LIST,
	FLOAT_LIST,
	BOOL_LIST,
	VOID_FUNCTION,
	INT_FUNCTION,
	FLOAT_FUNCTION,
	BOOL_FUNCTION,
};

class SymbolTableEntry
{
public:
	SymbolTableEntry() = default;

	bool isFunction();

	void setIdentifier(const std::string &identifier);
	std::string getIdentifier();

	void setType(Type type);
	Type getType();

	void setValue(const std::shared_ptr<void> &value);
	std::shared_ptr<void> getValue();

	void setFuncArgsNo(size_t number);
	size_t getFuncArgsNo();

	static std::string typeToString(Type type);

private:
	std::string identifier;
	Type type;
	std::shared_ptr<void> value;
	size_t funcArgsNo;
};

#endif
