#ifndef DERIVATION_TREE_HPP
#define DERIVATION_TREE_HPP

#include "DerivationNode.hpp"
#include "SymbolTable.hpp"

#include <vector>

class DerivationTree
{
public:
	DerivationTree() = default;
	DerivationTree(const std::shared_ptr<DerivationNode> &root);

	std::shared_ptr<DerivationNode> getParent();

	void pushTable(const std::shared_ptr<SymbolTable> &table);
	void execute();

	void fillSymbolTables();
	void printTree();
	void printSymbolTables();

private:
	void setType(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	void setArgsNo(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	void setValueMaybe(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	Type deduceType(bool isList, bool isFunction, const std::string &type);

	std::shared_ptr<DerivationNode> root;
	std::shared_ptr<DerivationNode> current;
	std::vector<std::shared_ptr<SymbolTable>> symbolTables;
};

#endif