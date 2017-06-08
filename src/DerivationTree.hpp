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

	void pushTable(const std::shared_ptr<SymbolTable> &table);
	std::vector<std::shared_ptr<SymbolTable>> getTables();

	void print();

private:
	std::shared_ptr<DerivationNode> root;
	std::vector<std::shared_ptr<SymbolTable>> symbolTables;
};

#endif
