#ifndef VALUE_HPP
#define VALUE_HPP

#include <memory>
#include <iostream>

enum class Type
{
	INT,
	FLOAT,
	BOOL,
};

class Value
{
public:
	Value() = default;
	Value(Type type);

	Value &operator=(const Value &val) {
		if (&val == this)
			return *this;

		type = val.getType();
		intVal = val.getInt();
		floatVal = val.getFloat();
		boolVal = val.getBool();
	};

	Value &operator*=(const Value &val) {
		if (&val == this)
			return *this;

		auto resultType = chooseWiderType(type, val.getType());
		auto numberValues = extractActualValues(this, &val);

		switch (resultType) {
		case Type::INT: {
			auto val1 = *(reinterpret_cast<int*>(numberValues.first));
			auto val2 = *(reinterpret_cast<int*>(numberValues.second));
			setInt(std::make_shared<int>(val1 * val2));
			return *this;
		}

		case Type::FLOAT: {
			auto val1 = *(reinterpret_cast<float*>(numberValues.first));
			auto val2 = *(reinterpret_cast<float*>(numberValues.second));
			setFloat(std::make_shared<float>(val1 * val2));
			return *this;
		}

		case Type::BOOL: {
			auto val1 = *(reinterpret_cast<bool*>(numberValues.first));
			auto val2 = *(reinterpret_cast<bool*>(numberValues.second));
			setBool(std::make_shared<bool>(val1 && val2));
			return *this;
		}
		}

		return *this;
	};

	Value &operator/=(const Value &val) {
		if (&val == this)
			return *this;

		auto resultType = chooseWiderType(type, val.getType());
		auto numberValues = extractActualValues(this, &val);

		switch (resultType) {
		case Type::INT: {
			auto val1 = *(reinterpret_cast<int*>(numberValues.first));
			auto val2 = *(reinterpret_cast<int*>(numberValues.second));
			setInt(std::make_shared<int>(val1 / val2));
			return *this;
		}

		case Type::FLOAT: {
			auto val1 = *(reinterpret_cast<float*>(numberValues.first));
			auto val2 = *(reinterpret_cast<float*>(numberValues.second));
			setFloat(std::make_shared<float>(val1 / val2));
			return *this;
		}

		case Type::BOOL: {
			auto val1 = *(reinterpret_cast<bool*>(numberValues.first));
			auto val2 = *(reinterpret_cast<bool*>(numberValues.second));
			setBool(std::make_shared<bool>(val1 / val2));
			return *this;
		}
		}

		return *this;
	};

	Value &operator+=(const Value &val) {
		if (&val == this)
			return *this;

		auto resultType = chooseWiderType(type, val.getType());
		auto numberValues = extractActualValues(this, &val);

		switch (resultType) {
		case Type::INT: {
			auto val1 = *(reinterpret_cast<int*>(numberValues.first));
			auto val2 = *(reinterpret_cast<int*>(numberValues.second));
			setInt(std::make_shared<int>(val1 + val2));
			return *this;
		}

		case Type::FLOAT: {
			auto val1 = *(reinterpret_cast<float*>(numberValues.first));
			auto val2 = *(reinterpret_cast<float*>(numberValues.second));
			setFloat(std::make_shared<float>(val1 + val2));
			return *this;
		}

		case Type::BOOL: {
			auto val1 = *(reinterpret_cast<bool*>(numberValues.first));
			auto val2 = *(reinterpret_cast<bool*>(numberValues.second));
			setBool(std::make_shared<bool>(val1 + val2));
			return *this;
		}
		}

		return *this;
	};

	Value &operator-=(const Value &val) {
		if (&val == this)
			return *this;

		auto resultType = chooseWiderType(type, val.getType());
		auto numberValues = extractActualValues(this, &val);

		switch (resultType) {
		case Type::INT: {
			auto val1 = *(reinterpret_cast<int*>(numberValues.first));
			auto val2 = *(reinterpret_cast<int*>(numberValues.second));
			setInt(std::make_shared<int>(val1 - val2));
			return *this;
		}

		case Type::FLOAT: {
			auto val1 = *(reinterpret_cast<float*>(numberValues.first));
			auto val2 = *(reinterpret_cast<float*>(numberValues.second));
			setFloat(std::make_shared<float>(val1 - val2));
			return *this;
		}

		case Type::BOOL: {
			auto val1 = *(reinterpret_cast<bool*>(numberValues.first));
			auto val2 = *(reinterpret_cast<bool*>(numberValues.second));
			setBool(std::make_shared<bool>(val1 - val2));
			return *this;
		}
		}

		return *this;
	};

	bool operator<(const Value &val) const;
	bool operator>(const Value &val) const;
	bool operator==(const Value &val) const;
	bool operator!=(const Value &val) const;
	bool operator&&(const Value &val) const;
	bool operator||(const Value &val) const;

	void setType(Type type);
	Type getType() const;

	void setInt(const std::shared_ptr<int> &val);
	std::shared_ptr<int> getInt() const;

	void setFloat(const std::shared_ptr<float> &val);
	std::shared_ptr<float> getFloat() const;

	void setBool(const std::shared_ptr<bool> &val);
	std::shared_ptr<bool> getBool() const;

	std::string typeToString();

private:
	Type chooseWiderType(Type type1, Type type2) const;
	std::pair<void*, void*> extractActualValues(const Value *val1, const Value *val2) const;

	Type type;
	std::shared_ptr<int> intVal;
	std::shared_ptr<float> floatVal;
	std::shared_ptr<bool> boolVal;
};

#endif
