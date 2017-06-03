#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include "SymbolTableEntry.hpp"
#include "DerivationNode.hpp"

#include <unordered_map>

class DerivationNode;
class SymbolTable
{
public:
	void addEntry(const std::string &name,
	         const std::shared_ptr<SymbolTableEntry> &entry,
			 const std::weak_ptr<DerivationNode> &node);

	std::shared_ptr<SymbolTableEntry> getEntry(const std::string &name);
	std::shared_ptr<DerivationNode> getNode(const std::string &name);

	std::unordered_map<std::string, std::pair<std::shared_ptr<SymbolTableEntry>, std::weak_ptr<DerivationNode>>> getScope();

private:
	std::unordered_map<std::string, std::pair<std::shared_ptr<SymbolTableEntry>, std::weak_ptr<DerivationNode>>> scope;
};

#endif
