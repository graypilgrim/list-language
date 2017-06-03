#include "DerivationTree.hpp"

#include <iostream>

DerivationTree::DerivationTree(const std::shared_ptr<DerivationNode> &root)
	: root(root)
{}

void DerivationTree::pushTable(const std::shared_ptr<SymbolTable> &table)
{
	symbolTables.emplace_back(table);
}

void DerivationTree::execute()
{

}

void DerivationTree::fillSymbolTables()
{
	for (auto &i : symbolTables) {
		for (auto &entry : i->getScope()) {
			
		}
	}
}

void DerivationTree::printTree()
{
	std::vector<std::shared_ptr<DerivationNode>> stack;
	stack.push_back(root);

	while (!stack.empty()) {
		auto node = stack.back();
		stack.pop_back();

		for (size_t i = 0 ; i < node->getDepth(); ++i)
			std::cout << " ";
		std::cout << node->getLabel() << std::endl;

		auto children = node->getChildren();

		for (auto n = children.rbegin(); n != children.rend(); ++n)
			stack.push_back(*n);
	}
}

void DerivationTree::printSymbolTables()
{
	for (auto &i : symbolTables) {
		std::cout << "Table: " << std::endl;
		for (auto &entry : i->getScope())
			std::cout << entry.first << " ";
		std::cout << std::endl;
	}
}
