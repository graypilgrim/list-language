#ifndef ITEM_DESCRIPTOR
#define ITEM_DESCRIPTOR

#include <string>

enum class ItemSpecificity {
	Function,
	Variable
};

class ItemDescriptor
{
private:
	ItemSpecificity specificity;
	std::string type;
	std::string name;
	size_t lineNumber;
};

#endif
