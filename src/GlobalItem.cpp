#include "GlobalItem.hpp"

GlobalItem::GlobalItem(const ItemSpecificity &specificity,
		   const std::string &type,
		   const std::string &name,
		   const size_t &lineNumber)
	: specificity(specificity), type(type), name(name), lineNumber(lineNumber)
{}

ItemSpecificity GlobalItem::getSpecificity() {
	return specificity;
}

void GlobalItem::setSpecificity(const ItemSpecificity &specificity) {
	this->specificity = specificity;
}

std::string GlobalItem::getType() {
	return type;
}

void GlobalItem::setType(const std::string &type) {
	this->type = type;
}

std::string GlobalItem::getName() {
	return name;
}

void GlobalItem::setName(const std::string &name) {
	this->name = name;
}

size_t GlobalItem::getLineNumber() {
	return lineNumber;
}

void GlobalItem::setLineNumber(const size_t &lineNumber) {
	this->lineNumber = lineNumber;
}
