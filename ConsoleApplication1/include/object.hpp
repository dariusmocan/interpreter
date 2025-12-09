#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <string>

using objectType = std::string;

class Object {
public:
	virtual ~Object() = default;

	virtual objectType Type();
	virtual std::string Inspect();
};




#endif // OBJECT_HPP
