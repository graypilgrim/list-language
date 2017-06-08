#include "DerivationTree.hpp"

#include <iostream>

DerivationTree::DerivationTree(const std::shared_ptr<DerivationNode> &root)
	: root(root)
{}

void DerivationTree::pushTable(const std::shared_ptr<SymbolTable> &table)
{
	symbolTables.emplace_back(table);
}

std::vector<std::shared_ptr<SymbolTable>> DerivationTree::getTables()
{
	return symbolTables;
}

void DerivationTree::print()
{
	std::vector<std::shared_ptr<DerivationNode>> stack;
	stack.push_back(root);

	while (!stack.empty()) {
		auto node = stack.back();
		stack.pop_back();

		for (size_t i = 0 ; i < node->getDepth(); ++i)
			std::cout << "-";
		std::cout << "|";
		if (node->getType() == NodeType::symbol)
			std::cout << node->getLabel();
		else
			std::cout << DerivationNode::nodeTypeToString(node->getType());
		std::cout << std::endl;

		auto children = node->getChildren();

		for (auto n = children.rbegin(); n != children.rend(); ++n)
			stack.push_back(*n);
	}
}
