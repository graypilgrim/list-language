#include "DerivationNode.hpp"

DerivationNode::DerivationNode(const std::string &label)
	: label(label), parent(std::weak_ptr<DerivationNode>())
{}

DerivationNode::DerivationNode(const std::string &label, const std::weak_ptr<DerivationNode> &parent)
	: label(label), parent(parent)
{}

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

void DerivationNode::addChild(const std::shared_ptr<DerivationNode> &child)
{
	children.push_back(child);
}
