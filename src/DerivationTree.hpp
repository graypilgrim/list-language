#ifndef DERIVATION_TREE_HPP
#define DERIVATION_TREE_HPP

#include "DerivationNode.hpp"

class DerivationTree
{
public:
	DerivationTree() = default;
	DerivationTree(const std::shared_ptr<DerivationNode> &root);
	void execute();

private:
	std::shared_ptr<DerivationNode> root;
	std::shared_ptr<DerivationNode> current;
};

#endif
