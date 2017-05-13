#ifndef GLOBAL_ITEM
#define GLOBAL_ITEM

#include <string>

enum class ItemSpecificity {
	Function,
	Variable
};

class GlobalItem
{
public:
	GlobalItem() = default;
	GlobalItem(const ItemSpecificity &specificity,
	           const std::string &type,
	           const std::string &name,
	           const size_t &lineNumber);

	ItemSpecificity getSpecificity();
	void setSpecificity(const ItemSpecificity &specificity);

	std::string getType();
	void setType(const std::string &type);

	std::string getName();
	void setName(const std::string &name);

	size_t getLineNumber();
	void setLineNumber(const size_t &lineNumber);

private:
	ItemSpecificity specificity;
	std::string type;
	std::string name;
	size_t lineNumber;
};

#endif
