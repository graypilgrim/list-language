#include "SymbolTable.hpp"

void SymbolTable::addEntry(const std::string &name,
		 const std::shared_ptr<SymbolTableEntry> &entry,
		 const std::shared_ptr<DerivationNode> &node)
{
	scope.emplace(name, std::make_pair(entry, node));
}

std::shared_ptr<SymbolTableEntry> SymbolTable::getEntry(const std::string &name)
{
	return scope.at(name).first;
}

std::shared_ptr<DerivationNode> SymbolTable::getNode(const std::string &name)
{
	return scope.at(name).second;
}
