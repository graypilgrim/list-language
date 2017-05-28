#ifndef DERIVATION_NODE_HPP
#define DERIVATION_NODE_HPP

#include <vector>
#include <memory>

class DerivationNode
{
public:
	DerivationNode(const std::string &label);
	DerivationNode(const std::string &label, const std::weak_ptr<DerivationNode> &parent);

	std::string getLabel() const;
	std::shared_ptr<DerivationNode> getParent() const;
	std::vector<std::shared_ptr<DerivationNode>> getChildren() const;

	void addChild(const std::shared_ptr<DerivationNode> &child);

private:
	std::string label;

	std::weak_ptr<DerivationNode> parent;
	std::vector<std::shared_ptr<DerivationNode>> children;
};

#endif
