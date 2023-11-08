#include "Object.h"

Object::Object() {  }

Object::Object(Object&& other) {
	container = other.container;
	other.container = nullptr;
	size = other.size;
	typeIdentifier = other.typeIdentifier;
	_typeName = other._typeName;
	destructor = std::move(other.destructor);
	
}

Object::Object(const Object& other) {
	container = malloc(other.size);

	if (container == nullptr)
		throw "Memory allocation error";

	size = other.size;
	typeIdentifier = other.typeIdentifier;
	_typeName = other._typeName;
	destructor = other.destructor;
	memcpy(container, other.container, size);
	
}


Object& Object::operator=(const Object& other) {
	if(destructor != nullptr) destructor(container);
	container = malloc(other.size);
	if (container == nullptr)
		throw "Memory allocation error";
	size = other.size;
	memcpy(container, other.container, size);
	typeIdentifier = other.typeIdentifier;
	_typeName = other._typeName;
	destructor = other.destructor;
	return *this;
}

Object& Object::operator=(Object&& other) {
	if(destructor != nullptr) destructor(container);
	container = other.container;
	other.container = nullptr;
	size = other.size;
	typeIdentifier = other.typeIdentifier;
	_typeName = other._typeName;
	destructor = other.destructor;
	return *this;
}


void* Object::getValue() const {
	return container;
}

size_t Object::identifier() const
{
	return typeIdentifier;
}

const char* Object::typeName() const
{
	return _typeName;
}

Object::~Object() {
	if(destructor != nullptr) destructor(container);
}


