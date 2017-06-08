#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "DerivationTree.hpp"

#include <vector>
#include <memory>

class Interpreter
{
public:
	Interpreter(const std::shared_ptr<DerivationTree> &tree);

	void execute();

	void printSymbolTables();

private:
	void fillSymbolTables();

	void setType(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	void setArgsNo(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	void setValueMaybe(const std::shared_ptr<DerivationNode> &node, const std::shared_ptr<SymbolTableEntry> &entry);
	Type deduceType(bool isList, bool isFunction, const std::string &type);
	std::shared_ptr<DerivationNode> findMain();
	void nextNode();
	void assignValue(const std::shared_ptr<DerivationNode> &n = nullptr);
	void processStmt();
	void processIf();
	void processFor();
	void processListStmt();
	void processIndexStmt();
	void processPrintStmt();
	std::shared_ptr<Value> evaluate(const std::shared_ptr<DerivationNode> &node);
	std::shared_ptr<Value> evaluateVal(const std::shared_ptr<DerivationNode> &node);
	std::shared_ptr<Value> evaluateMul(const std::shared_ptr<DerivationNode> &node);
	std::shared_ptr<Value> evaluateSum(const std::shared_ptr<DerivationNode> &node);
	std::shared_ptr<Value> evaluateComp(const std::shared_ptr<DerivationNode> &node);
	std::shared_ptr<Value> evaluateAnd(const std::shared_ptr<DerivationNode> &node);

	std::shared_ptr<DerivationTree> tree;
	std::shared_ptr<DerivationNode> current;
	std::vector<std::shared_ptr<DerivationNode>> dfsStack;
};

#endif
