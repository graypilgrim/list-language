#include "Value.hpp"

Value::Value(Type type)
	: type(type)
{}

inline bool Value::operator<(const Value &val)
{
	if (type != val.getType())
		throw std::runtime_error("Comparison of different types");

	switch (type) {
	case Type::INT:
		return *getInt() < *(val.getInt());

	case Type::FLOAT:
		return *getFloat() < *(val.getFloat());

	case Type::BOOL:
		return *getBool() < *(val.getBool());
	}
}

inline bool Value::operator>(const Value &val)
{

}

inline bool Value::operator==(const Value &val)
{

}

void Value::setType(Type type)
{
	this->type = type;
}

Type Value::getType() const
{
	return type;
}

void Value::setInt(const std::shared_ptr<int> &val)
{
	intVal = val;
}

std::shared_ptr<int> Value::getInt() const
{
	return intVal;
}

void Value::setFloat(const std::shared_ptr<float> &val)
{
	floatVal = val;
}

std::shared_ptr<float> Value::getFloat() const
{
	return floatVal;
}

void Value::setBool(const std::shared_ptr<bool> &val)
{
	boolVal = val;
}

std::shared_ptr<bool> Value::getBool() const
{
	return boolVal;
}

std::string Value::typeToString()
{
	switch (type) {
	case Type::INT:
		return "INT";

	case Type::FLOAT:
		return "FLOAT";

	case Type::BOOL:
		return "BOOL";
	}

	return "NONE";
}

Type Value::chooseWiderType(Type type1, Type type2) const
{
	if (type1 == type2)
		return type1;

	if (type1 == Type::FLOAT || type2 == Type::FLOAT)
		return Type::FLOAT;

	if (type1 == Type::INT || type2 == Type::INT)
		return Type::INT;

	return Type::BOOL;
}

std::pair<void*, void*> Value::extractActualValues(const Value *val1, const Value *val2) const
{
	void *result1;
	switch (val1->getType()) {
	case Type::INT:
		result1 = getInt().get();
		break;

	case Type::FLOAT:
		result1 = getFloat().get();
		break;

	case Type::BOOL:
		result1 = getBool().get();
		break;
	}

	void *result2;
	switch (val2->getType()) {
	case Type::INT:
		result2 = getInt().get();
		break;

	case Type::FLOAT:
		result2 = getFloat().get();
		break;

	case Type::BOOL:
		result2 = getBool().get();
		break;
	}

	return std::make_pair(result1, result2);
}
