#ifndef DERIVATION_NODE_HPP
#define DERIVATION_NODE_HPP

#include "SymbolTable.hpp"

#include <vector>
#include <memory>
#include <unordered_map>

enum class NodeType {
	program,
	funDef,
	funDecls,
	funDecl,
	args,
	stmts,
	stmt,
	condStmt,
	whileStmt,
	forStmt,
	callStmt,
	indexStmt,
	varDef,
	varDecl,
	listStmt,
	basicListIter,
	listIfStmt,
	retStmt,
	assignStmt,
	ifStmt,
	elseStmt,
	printStmt,
	expr,
	andExpr,
	compExpr,
	sumExpr,
	mulExpr,
	val,
	type,
	identifier,
	semicolon,
	symbol
};

class SymbolTable;
class DerivationNode
{
public:
	DerivationNode(NodeType type);
	DerivationNode(NodeType type, const std::weak_ptr<DerivationNode> &parent);

	NodeType getType();
	std::string getLabel() const;
	void setLabel(const std::string &label);
	size_t getDepth() const;
	bool wasVisited();
	void setVisited();
	std::shared_ptr<DerivationNode> getParent() const;
	std::vector<std::shared_ptr<DerivationNode>> getChildren() const;
	void removeChild(size_t index);

	void addChild(const std::shared_ptr<DerivationNode> &child);

	std::shared_ptr<SymbolTable> createScope();
	std::shared_ptr<SymbolTable> getSymbolTable();
	std::shared_ptr<SymbolTableEntry> getSymbolEntry(const std::string &symbol);

	size_t findIndexInParent();

	static std::string nodeTypeToString(NodeType type);

private:
	static size_t index;

	NodeType type;
	std::string label;
	size_t depth;
	bool visited;

	std::weak_ptr<DerivationNode> parent;
	std::vector<std::shared_ptr<DerivationNode>> children;

	std::shared_ptr<SymbolTable> localScope;
};

#endif
