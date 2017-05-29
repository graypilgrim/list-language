#ifndef DERIVATION_TREE_HPP
#define DERIVATION_TREE_HPP

#include "DerivationNode.hpp"
#include "SymbolTable.hpp"

#include <unordered_map>

class DerivationTree
{
public:
	DerivationTree() = default;
	DerivationTree(const std::shared_ptr<DerivationNode> &root);
	void execute();
	void print();

private:
	std::shared_ptr<DerivationNode> root;
	std::shared_ptr<DerivationNode> current;
};

#endif
