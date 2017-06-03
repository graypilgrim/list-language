#ifndef PARSER_HPP
#define PARSER_HPP

#include "GlobalItem.hpp"
#include "Lexer.hpp"
#include "DerivationTree.hpp"
#include "SymbolTableEntry.hpp"

#include <vector>
#include <set>
#include <unordered_map>
#include <memory>
#include <stdexcept>

class Parser
{
public:
	Parser();
	Parser(const std::shared_ptr<Lexer> &lexer);
	std::shared_ptr<DerivationTree> run();

private:
	std::shared_ptr<DerivationNode> nextAtom(const std::weak_ptr<DerivationNode> &parent);
	std::string currentLine();

	std::shared_ptr<DerivationNode> funDef(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> funsDecls(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> funDecl(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> args(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> stmts(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> stmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> condStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> whileStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> forStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> callStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> indexStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> varDef(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> varDecl(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> listStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> basicListIter(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> listIfStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> retStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> assignStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> ifStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> elseStmt(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> expr(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> andExpr(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> compExpr(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> sumExpr(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> mulExpr(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> val(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> type(const std::weak_ptr<DerivationNode> &parent);
	std::shared_ptr<DerivationNode> identifier(const std::weak_ptr<DerivationNode> &parent);

	std::shared_ptr<DerivationNode> semicolon(const std::weak_ptr<DerivationNode> &parent);

	inline bool isAtomNumber();
	inline bool isAtomBoolVal();
	inline bool isAtomCompOperator();
	inline bool isAtomSumOperator();
	inline bool isAtomMulOperator();
	inline bool isAtomType();

	std::string atom;

	std::shared_ptr<Lexer> lexer;
	std::set<std::string> keywords;
	std::set<std::string> types;
	std::shared_ptr<DerivationTree> tree;
};

#endif
