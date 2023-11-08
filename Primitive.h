#pragma once
#include "Object.h"
#include <typeinfo>
#include <string>
#include <sstream>
#include <fstream>



template <typename T>
concept primitive = std::is_fundamental<T>::value;


template <primitive T>
class Primitive :
    public Object
{
private:

	T convertContainer(void* value) const;

public:

	static const size_t identifier;

	Primitive();
	Primitive(T value);
	Primitive(const Primitive<T>& value);
	Primitive(Primitive<T>&& value);
	Primitive<T>& operator=(const Primitive<T>& other);
	Primitive<T>& operator=(Primitive<T>&& other);
	T getValue() const;

	void initialize() override;
	//Returns the string representation of value. [!] Need to be freed.
	char* to_cstring() const override;
	void from_cstring(const char*) override;
	void writeBinary(std::ofstream&) const override;
	void readBinary(std::ifstream&) override;
	Object* getCopy() const override;
	std::partial_ordering compare(const Object& other) const override; 
	void add(const Object& other) override; 

	operator T&();

};
template<primitive T>
const size_t Primitive<T>::identifier = typeid(Primitive<T>).hash_code();;

template<primitive T>
inline T Primitive<T>::convertContainer(void* value) const
{
	return *((T*)value);
};

template<primitive T>
inline Primitive<T>::Primitive() : Object(T())
{
	initialize();
};

template<primitive T>
inline Primitive<T>::Primitive(T value) : Object(value)
{
	initialize();
};

template<primitive T>
inline Primitive<T>::Primitive(const Primitive<T>& value) : Object((Object&)value)
{
	initialize();
};

template<primitive T>
inline Primitive<T>::Primitive(Primitive<T>&& value) : Object((Object&&)value)
{
	initialize();
};

template<primitive T>
inline Primitive<T>& Primitive<T>::operator=(const Primitive<T>& other)
{
	Object::operator=(other);
	return *this;
}


template<primitive T>
inline Primitive<T>& Primitive<T>::operator=(Primitive<T>&& other)
{
	Object::operator=(std::move(other));
	return *this;
};

template<primitive T>
inline T Primitive<T>::getValue() const
{
	return *((T*)Object::getValue());
};

template<primitive T>
inline void Primitive<T>::initialize()
{
	_typeName = typeid(*this).name();
	Object::typeIdentifier = typeid(*this).hash_code();
};

template<primitive T>
inline char* Primitive<T>::to_cstring() const
{
	std::string value = std::to_string(getValue());
	const char* str = value.c_str();
	char* nonConstStr = new char[strlen(str) + 1];
	strcpy_s(nonConstStr, strlen(str)+1, str);
	return nonConstStr;

};

template<primitive T>
inline void Primitive<T>::from_cstring(const char* str)
{
	T value = 0;
	std::istringstream(std::string(str)) >> value;
	setContainerValue(value);

};

template<primitive T>
inline void Primitive<T>::writeBinary(std::ofstream& os) const
{
	T value = getValue();
	os.write((char*)&value, sizeof(T));
	os.flush();

};

template<primitive T>
inline void Primitive<T>::readBinary(std::ifstream& is)
{
	T value = 0;
	is.read((char*)&value, sizeof(T));
	setContainerValue(value);
};

template<primitive T>
inline Object* Primitive<T>::getCopy() const
{
	return new Primitive<T>(*this);
};

template<primitive T>
inline std::partial_ordering Primitive<T>::compare(const Object& other) const
{
	typeIdentifier;
	if ((Object::identifier()) != (other.identifier())) return std::partial_ordering::unordered;
	return getValue() <=> convertContainer(other.getValue());
};

template<primitive T>
inline void Primitive<T>::add(const Object& other)
{
	if (Object::identifier() != other.identifier()) return;
	setContainerValue(getValue() + convertContainer(other.getValue()));

};

template<primitive T>
inline Primitive<T>::operator T& ()
{
	return *(T*)Object::getValue();
};


typedef Primitive<float> Float;
typedef Primitive<int> Int;
typedef Primitive<char> Char;
typedef Primitive<double> Double;