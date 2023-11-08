#pragma once
#include <iostream>
#include <functional>
template <typename T>
class clist
{
private:
	friend class iterator;
	struct Node {
		Node* next = nullptr;
		Node* prev = nullptr;
		T value;
		Node() = default;
		Node(T value) {
			this->value = value;
		}
	};
	
	Node* first = nullptr;
	size_t _size = 0;

public:
	
	//typedef T& elem;
	class iterator;
	class const_iterator;

	clist();
	clist(std::initializer_list<T>);
	clist(const clist<T>& other);
	clist(clist<T>&& other);
	
	clist& operator=(const clist<T>&);
	clist& operator=(clist<T>&&);

	//Inserts the element as the last one
	void push_back(T);
	//Inserts the element as the first one
	void push_front(T);
	//Inserting the value before the node iterator points on
	void insert(iterator, T);
	//Erasing the values in [begin; end)
	void erase(iterator begin, iterator end);
	//Erasing the value iterator points on
	void erase(iterator);
	//Deletes all the values from cycled list
	void clear();
	//Deletes the last element
	void pop_back();
	//Deletes the first element
	void pop_front();
	
	// Scrolls the list left (the second element becomes the first, the first becomes the last)
	void shift_forward();
	// Scrolls the list right (the first element becomes the second, the last element becomes the first)
	void shift_backward();
	
	// Performs the received function for every element in list. Function has to be void and get clist value or it's reference to it as an argument: void f(<type>& value) 
	auto foreach(std::function<void(T&)>);
	// Performs the received function for every element in list. Function has to be void and get const clist value or reference to it as an argument: void f(const <type>& value)
	auto foreach(std::function<void(const T&)>) const;

	//shows is clist empty
	bool empty() const;
	//returns the current clist size
	size_t size() const;

	//Returns a reference to the first element in cycled list
	T& front();
	//Returns a reference to the last element in cycled list
	T& back();
	//Returns a const reference to the first element in cycled list
	const T& front() const;
	//Returns a const reference to the last element in cycled list
	const T& back() const;
	
	//Returns const iterator points to the first element in cycled list
	const_iterator begin() const;

	//Returns iterator points to the first element in cycled list
	iterator begin();

	~clist();

	// Iterator class
	class iterator {
	private:
		Node* ptr = nullptr;
		const clist<T>* associatedContainer = nullptr;
		friend class clist<T>;
		Node* node() const;
	public:
		iterator();
		iterator(Node* ptr, const clist<T>* container);
		iterator(const iterator& other);
		//Returns a reference to the element iterator is pointing at
		T& operator*();
		iterator operator++(int);
		iterator& operator++();
		iterator& operator--();
		iterator operator--(int);
		bool operator==(const iterator& other) const;
		bool operator==(const clist<T>::const_iterator& other) const;

		iterator& operator=(const iterator& other);
		iterator& operator=(iterator&& other);

	};
	//Const iterator
	class const_iterator {
	private:
		Node* ptr = nullptr;
		const clist<T>* associatedContainer = nullptr;
		friend class clist<T>;
		friend class clist<T>::iterator;
		Node* const node() const;
	public:
		const_iterator();
		const_iterator(Node* ptr, const clist<T>* container);
		const_iterator(const const_iterator& other);
		const_iterator(const iterator& other);
		//Returns the reference to the element iterator is pointing at. Value can't be changed.
		const T& operator*() const;
		const_iterator operator++(int);
		const_iterator& operator++();
		const_iterator operator--(int);
		const_iterator& operator--();
		bool operator==(const const_iterator& other) const;
		const_iterator& operator=(const const_iterator& other);
		const_iterator& operator=(const_iterator&& other);
	};

};


template<typename T>
inline clist<T>::clist()
{
	
}

template<typename T>
inline clist<T>::clist(std::initializer_list<T> args)
{
	auto end = args.end();
	for (auto i = args.begin(); i != end; i++) {
		push_back(*i);
	}
}

template<typename T>
inline clist<T>::clist(const clist<T>& other)
{
	other.foreach([&](T val) {
		this->push_back(val);
	});
}

template<typename T>
inline clist<T>::clist(clist<T>&& other)
{
	first = other.first;
	other.first = nullptr;
}

template<typename T>
inline clist<T>& clist<T>::operator=(const clist<T>& other)
{
	clear();
	auto it = other.begin();
	if (!other.empty()) {
		do {
			push_back(*it);
			it++;
		} while (it != other.begin());
	}
	return *this;
}

template<typename T>
inline clist<T>& clist<T>::operator=(clist<T>&& other)
{
	clear();

	first = other.first;
	_size = other._size;

	other.first = nullptr;
	other._size = 0;
	return *this;
}

template<typename T>
inline void clist<T>::push_back(T value)
{
	Node* newNode = new Node(value);
	//newNode->value = value;
	if (empty()) {
		newNode->next = newNode;
		newNode->prev = newNode;
		first = newNode;
	}
	else {
		newNode->next = first;
		newNode->prev = first->prev;
		first->prev->next = newNode;
		first->prev = newNode;
	}
	_size++;
}

template<typename T>
inline void clist<T>::push_front(T value)
{
	Node* newNode = new Node();
	newNode->value = value;
	if (empty()) {
		newNode->next = newNode;
		newNode->prev = newNode;
	}
	else {
		newNode->next = first;
		newNode->prev = first->prev;
		first->prev->next = newNode;
		first->prev = newNode;
	}
	first = newNode;
	_size++;
}

template<typename T>
inline void clist<T>::insert(iterator it, T val)
{
	iterator empty;

	if (it.associatedContainer != this) throw std::logic_error("insert: Iterator isn't linked to that container");

	if (it != empty) {


		Node* prev = it.node()->prev;
		Node* next = it.node();
		Node* newNode = new Node;

		prev->next = newNode;
		next->prev = newNode;

		newNode->value = val;
		newNode->next = next;
		newNode->prev = prev;

		_size++;

		if (it.node() == first)
			first = newNode;
	}
	else {
		if (_size == 0) {
			push_back(val);
		}
		else {
			throw std::logic_error("insert: invalid iterator ");
		}
	}
}

template<typename T>
inline void clist<T>::clear()
{
	iterator sk = begin();
	iterator sks = begin();
	erase(sk, sks);
}

template<typename T>
inline void clist<T>::erase(iterator begin, iterator end)
{
	iterator empty;
	if (begin == empty && begin.associatedContainer == this && end == empty && end.associatedContainer == this && _size == 0) return;
	if (begin.associatedContainer != this || end.associatedContainer != this) throw std::logic_error("erase: using of iterator isn't linked to that container");
	try {
		*begin;
		*end;
	}
	catch (std::logic_error err) {
		throw err;
	}

	do {
		
		iterator it_next = { begin.node()->next, begin.associatedContainer };
		iterator it_prev = --iterator(begin);
		it_prev.node()->next = it_next.node();
		it_next.node()->prev = it_prev.node();
		if (first == begin.node()) first = it_next.node();
		delete begin.node();
		begin = it_next;
		_size--;
		if (_size == 0) {
			first = nullptr;
			break;
		}

	} while (begin != end);
}

template<typename T>
inline void clist<T>::erase(iterator it)
{
	if (it != iterator()) {
		iterator start = it;
		iterator end = ++it;

		erase(start, end);
	}
}

template<typename T>
inline void clist<T>::pop_back()
{
	if (empty()) 
		throw std::logic_error("pop_back called on empty clist");
	else
		erase(--begin());
}

template<typename T>
inline void clist<T>::pop_front()
{
	if (empty()) 
		throw std::logic_error("pop_front called on empty clist");
	else
		erase(begin());
}

template<typename T>
inline void clist<T>::shift_forward()
{
	if (empty()) 
		throw std::logic_error("shift_forward called on empty clist");
	else
		first = first->next;
}

template<typename T>
inline void clist<T>::shift_backward()
{
	if(empty()) throw std::logic_error("shift_backward called on empty clist");
	first = first->prev;
}

template<typename T>
inline bool clist<T>::empty() const
{
	return _size == 0;
}

template<typename T>
inline size_t clist<T>::size() const
{
	return _size;
}

template<typename T>
inline clist<T>::Node* clist<T>::iterator::node() const
{
	if (ptr == nullptr) throw std::logic_error("Invalid iterator");
	return ptr;
}


template<typename T>
inline clist<T>::iterator::iterator()
{
}

template<typename T>
inline clist<T>::iterator::iterator(Node* ptr, const clist<T>* container)
{
	this->associatedContainer = container;
	this->ptr = ptr;
}

template<typename T>
inline clist<T>::iterator::iterator(const iterator& other)
{
	this->ptr = other.ptr;
	this->associatedContainer = other.associatedContainer;
}


template<typename T>
inline T& clist<T>::iterator::operator*()
{
	if (ptr == nullptr) 
		throw std::logic_error("*iter with invalid iterator");
	else
		return this->ptr->value;
}

template<typename T>
inline clist<T>::iterator clist<T>::iterator::operator++(int n)
{
	if (ptr == nullptr) {
		throw std::logic_error("iter++ with invalid iterator");
	}
	else
	{
		iterator temp = *this;
		if (n == 0) {
			++* this;
		}
		else {
			for (int i = 0; i < n; i++)
				++* this;
		}
		return temp;
	}
}

template<typename T>
inline clist<T>::iterator& clist<T>::iterator::operator++()
{
	if (ptr != nullptr) {
		ptr = ptr->next;
		return *this;
	}
	throw std::logic_error("++iter with invalid iterator");
}

template<typename T>
inline clist<T>::iterator& clist<T>::iterator::operator--()
{
	if (ptr == nullptr) {
		throw std::logic_error("--iter with invalid iterator");
	}
	else {
		ptr = ptr->prev;
		return *this;
	}
}

template<typename T>
inline clist<T>::iterator clist<T>::iterator::operator--(int n)
{
	if (ptr == nullptr) {
		throw std::logic_error("iter-- with invalid iterator");
	}
	else {
		iterator temp = *this;
		if (n == 0) {
			--* this;
		}
		else {
			for (int i = 0; i < n; i++) {
				--* this;
			}
		}
		return temp;
	}
}

template<typename T>
inline bool clist<T>::iterator::operator==(const iterator& other) const
{
	return ptr == other.ptr;
}

template<typename T>
inline bool clist<T>::iterator::operator==(const clist<T>::const_iterator& other) const
{
	return other.ptr == this->ptr;
}

template<typename T>
inline clist<T>::iterator& clist<T>::iterator::operator=(const iterator& other)
{
	ptr = other.ptr;
	associatedContainer = other.associatedContainer;
	return *this;
}

template<typename T>
inline clist<T>::iterator& clist<T>::iterator::operator=(iterator&& other)
{
	ptr = other.ptr;
	associatedContainer = other.associatedContainer;
	return *this;
}



template<typename T>
inline T& clist<T>::front()
{
	if (!empty())
		return first->value;
	throw std::logic_error("front() called on empty clist");
}

template<typename T>
inline T& clist<T>::back()
{
	if(!empty())
		return first->prev->value;
	throw std::logic_error("back() called on empty clist");
}

template<typename T>
inline const T& clist<T>::front() const
{
	if (!empty())
		return first->prev->value;
	throw std::logic_error("front() called on empty clist");
}

template<typename T>
inline const T& clist<T>::back() const
{
	if (!empty())
		return first->prev->value;
	throw std::logic_error("back() called on empty clist");
}

template<typename T>
inline clist<T>::const_iterator clist<T>::begin() const
{
	return const_iterator(first, this);
}

template<typename T>
inline clist<T>::iterator clist<T>::begin()
{
	return iterator(first, this);
}

template<typename T>
inline clist<T>::~clist()
{
	clear();
}


template<typename T>
inline auto clist<T>::foreach(std::function<void(T&)> lambda)
{
	if (!empty()) {

		iterator it = begin();
		do {
			lambda(*it);
			it++;
		} while (it != begin());
	}
}

template<typename T>
inline auto clist<T>::foreach(std::function<void(const T&)> lambda) const
{
	if (!empty()) {

		auto it = begin();
		do {
			lambda(*it);
			it++;
		} while (it != begin());
	}
}

template<typename T>
inline clist<T>::Node* const clist<T>::const_iterator::node() const
{
	return ptr;
}

template<typename T>
inline clist<T>::const_iterator::const_iterator()
{
}

template<typename T>
inline clist<T>::const_iterator::const_iterator(Node* ptr, const clist<T>* container)
{
	this->ptr = ptr;
	this->associatedContainer = container;
}

template<typename T>
inline clist<T>::const_iterator::const_iterator(const const_iterator& other)
{
	this->ptr = other.ptr;
	this->associatedContainer = other.associatedContainer;
}

template<typename T>
inline clist<T>::const_iterator::const_iterator(const iterator& other)
{
	this->ptr = other.ptr;
	this->associatedContainer = associatedContainer;

}

template<typename T>
inline const T& clist<T>::const_iterator::operator*() const
{
	return ptr->value;
}

template<typename T>
inline clist<T>::const_iterator clist<T>::const_iterator::operator++(int n)
{
	if (ptr == nullptr) throw std::logic_error("Invalid iterator");
	const_iterator temp = *this;
	if (n == 0) {
		++*this;
	}
	else {
		for (int i = 0; i < n; i++)
			++*this;
	}
	return temp;
}

template<typename T>
inline clist<T>::const_iterator& clist<T>::const_iterator::operator++()
{
	if (ptr != nullptr) {
		ptr = ptr->next;
		return *this;
	}
	throw std::logic_error("++iter with invalid iterator");
}

template<typename T>
inline clist<T>::const_iterator clist<T>::const_iterator::operator--(int n)
{
	if(ptr == nullptr) throw std::logic_error("iter-- with invalid iterator");

	const_iterator temp = *this;
	if (n == 0) {
		--*this;
	}
	else {
		for (int i = 0; i < n; i++) {
			--*this;
		}
	}
	return temp;
}

template<typename T>
inline clist<T>::const_iterator& clist<T>::const_iterator::operator--()
{
	if (ptr == nullptr) throw std::logic_error("--iter with invalid iterator");
	ptr = ptr->prev;
	return *this;
}

template<typename T>
inline bool clist<T>::const_iterator::operator==(const const_iterator& other) const
{
	return ptr == other.ptr;
}

template<typename T>
inline clist<T>::const_iterator& clist<T>::const_iterator::operator=(const const_iterator& other)
{
	ptr = other.ptr;
	associatedContainer = other.associatedContainer;
	return *this;
}

template<typename T>
inline clist<T>::const_iterator& clist<T>::const_iterator::operator=(const_iterator&& other)
{
	ptr = other.ptr;
	associatedContainer = other.associatedContainer;
	return *this;
}
