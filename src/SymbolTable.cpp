#include "SymbolTable.hpp"

void SymbolTable::addEntry(const std::string &name,
		 const std::shared_ptr<SymbolTableEntry> &entry,
		 const std::weak_ptr<DerivationNode> &node)
{
	scope.emplace(name, std::make_pair(entry, node));
}

std::shared_ptr<SymbolTableEntry> SymbolTable::getEntry(const std::string &name)
{
	std::shared_ptr<SymbolTableEntry> result;
	try
	{
		result = scope.at(name).first;
	}
	catch (...)
	{
		result = nullptr;
	}

	return result;
}

std::shared_ptr<DerivationNode> SymbolTable::getNode(const std::string &name)
{
	std::shared_ptr<DerivationNode> result;
	try
	{
		result = scope.at(name).second.lock();
	}
	catch (...)
	{
		result = nullptr;
	}
	
	return result;
}

std::unordered_map<std::string, std::pair<std::shared_ptr<SymbolTableEntry>, std::weak_ptr<DerivationNode>>> SymbolTable::getScope()
{
	return scope;
}
