#ifndef DERIVATION_NODE_HPP
#define DERIVATION_NODE_HPP

#include "SymbolTable.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

class SymbolTable;
class DerivationNode
{
public:
	DerivationNode(const std::string &label);
	DerivationNode(const std::string &label, const std::weak_ptr<DerivationNode> &parent);

	std::string getLabel() const;
	size_t getDepth() const;
	bool wasVisited();
	void setVisited();
	std::shared_ptr<DerivationNode> getParent() const;
	std::vector<std::shared_ptr<DerivationNode>> getChildren() const;

	void addChild(const std::shared_ptr<DerivationNode> &child);

	std::shared_ptr<SymbolTable> createScope();
	std::shared_ptr<SymbolTable> getSymbolTable();

	size_t findIndexInParent();

private:
	static size_t index;

	std::string label;
	size_t depth;
	bool visited;

	std::weak_ptr<DerivationNode> parent;
	std::vector<std::shared_ptr<DerivationNode>> children;

	std::shared_ptr<SymbolTable> localScope;
};

#endif
