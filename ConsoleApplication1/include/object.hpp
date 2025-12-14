#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

using objectType = std::string;

namespace objectTypes {
	const objectType INTEGER_OBJ = "INTEGER";
	const objectType BOOLEAN_OBJ = "BOOLEAN";
	const objectType NULL_OBJ = "NULL";
	const objectType RETURN_OBJ = "RETURN_VALUE";
	const objectType ERROR_OBJ = "ERROR";
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

class ReturnValue : public Object {
public:
	std::unique_ptr<Object> value;

	ReturnValue(std::unique_ptr<Object> val) : value(std::move(val)) {};
	
	objectType Type() const override {
		return objectTypes::RETURN_OBJ;
	}

	std::string Inspect() const override {
		return value->Inspect();
	}
};

class Error : public Object {
public:
	std::string message;

	Error(const std::string& mess) : message(mess) {};

	objectType Type() const override {
		return objectTypes::ERROR_OBJ;
	}

	std::string Inspect() const override {
		return std::string("ERROR : ") + message;
	}

};

class Environment {
public:
	std::unordered_map<std::string, std::shared_ptr<Object>> store;

	Environment() = default;
	std::pair<std::shared_ptr<Object>, bool> getObject(const std::string& name) {
		auto it = store.find(name);
		if (it == store.end()) {
			return { nullptr, false };
		}

		return { it->second, true };
	}

	std::shared_ptr<Object> setObject(const std::string& name, std::shared_ptr<Object> val) {
		store[name] = val;
		return val;
	}
};

#endif // OBJECT_HPP
