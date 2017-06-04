#include "DerivationNode.hpp"

#include <iostream>

DerivationNode::DerivationNode(const std::string &label)
	: label(label), depth(0), visited(false), parent(std::weak_ptr<DerivationNode>())
{
}

DerivationNode::DerivationNode(const std::string &label, const std::weak_ptr<DerivationNode> &parent)
	: label(label), visited(false), parent(parent)
{
	auto p = parent.lock();
	depth = p ? p->getDepth() + 1 : 0;
}

std::string DerivationNode::getLabel() const
{
	return label;
}

std::shared_ptr<DerivationNode> DerivationNode::getParent() const
{
	return parent.lock();
}

std::vector<std::shared_ptr<DerivationNode>> DerivationNode::getChildren() const
{
	return children;
}

size_t DerivationNode::getDepth() const
{
	return depth;
}

bool DerivationNode::wasVisited()
{
	return visited;
}

void DerivationNode::setVisited()
{
	visited = true;
}

void DerivationNode::addChild(const std::shared_ptr<DerivationNode> &child)
{
	children.push_back(child);
}

std::shared_ptr<SymbolTable> DerivationNode::createScope()
{
	localScope = std::make_shared<SymbolTable>();
	return localScope;
}

std::shared_ptr<SymbolTable> DerivationNode::getSymbolTable()
{
	if (localScope)
		return localScope;

	auto p = parent.lock();
	return p->getSymbolTable();
}
