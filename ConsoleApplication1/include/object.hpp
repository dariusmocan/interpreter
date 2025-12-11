#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>

using objectType = std::string;

namespace objectTypes {
	const objectType INTEGER_OBJ = "INTEGER";
	const objectType BOOLEAN_OBJ = "BOOLEAN";
	const objectType NULL_OBJ = "NULL";
}

class Object {
public:
	virtual ~Object() = default;

	virtual objectType Type() const = 0;
	virtual std::string Inspect() const = 0;
};

class Integer : public Object {
public:
	int64_t value;

	Integer(int64_t val) : value(val) {};
	objectType Type() const override {
		return objectTypes::INTEGER_OBJ;
	}

	std::string Inspect() const override {
		return std::to_string(value);
	}
};

class Boolean : public Object {
public:
	bool value;

	Boolean(bool val) : value(val) {};

	objectType Type() const override {
		return objectTypes::BOOLEAN_OBJ;
	}

	std::string Inspect() const override {
		return value ? "true" : "false";
	}
};

class Null : public Object {
public:
	objectType Type() const override {
		return objectTypes::NULL_OBJ;
	}

	std::string Inspect() const override {
		return "null";
	}
};





#endif // OBJECT_HPP
