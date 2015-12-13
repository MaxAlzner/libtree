#pragma once

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

#include <vector>

template <typename T> struct binarynode_t;
template <typename T> struct binaryiterator_t;
template <typename T> class binarytree_t;

template <typename T> struct binarynode_t
{
	
	inline binarynode_t() :
		_left(0),
		_right(0),
		_parent(0),
		_tree(0),
		_ring(-1),
		_data(0) {}
	inline binarynode_t(const binarytree_t<T>* tree, const int32_t ring, const T* data) :
		_left(0),
		_right(0),
		_parent(0),
		_tree(tree),
		_ring(ring),
		_data(data) {}
	inline ~binarynode_t() {}
	
	binarynode_t<T>* _left;
	binarynode_t<T>* _right;
	binarynode_t<T>* _parent;
	const binarytree_t<T>* _tree;
	int32_t _ring;
	const T* _data;
	
};

template <typename T> struct binaryiterator_t
{
	
	inline binaryiterator_t() : _node(0) {}
	inline binaryiterator_t(const binarynode_t<T>& node) : _node(&node) {}
	inline ~binaryiterator_t() {}
	
	inline bool has_left() const;
	inline bool has_right() const;
	inline bool root() const;
	inline bool leaf() const;
	
	inline binaryiterator_t<T>& operator++();
	inline binaryiterator_t<T>& operator--();
	inline T& operator*() const;
	// inline bool operator<(const binaryiterator_t<T>& other) const;
	// inline bool operator>(const binaryiterator_t<T>& other) const;
	// inline bool operator<=(const binaryiterator_t<T>& other) const;
	// inline bool operator>=(const binaryiterator_t<T>& other) const;
	inline bool operator==(const binaryiterator_t<T>& other) const;
	inline bool operator!=(const binaryiterator_t<T>& other) const;
	
	const binarynode_t<T>* _node;
	
};

template <typename T> class binarytree_t
{
public:
	
	// template <typename U> struct node_t;
	// template <typename U> struct iterator_t;
	
	typedef binaryiterator_t<T> iterator;
	
	inline binarytree_t() : _rings(0), _root(0) {}
	inline ~binarytree_t() { this->clear(); }
	
	// friend struct binarynode_t<T>;
	friend struct binaryiterator_t<T>;
	
	// typedef binaryiterator_t<T> iterator;
	// typedef binarynode_t<T> node;
	
	inline iterator set_root(const T& item);
	
	inline iterator root() const { return iterator(this->_root); }
	inline void clear();
	
protected:
	
	// void register(const node_t* parent, const T* item);
	
	std::vector<binarynode_t<T> > _registry;
	uint32_t _rings;
	binarynode_t<T>* _root;
	
};

template <typename T> inline binaryiterator_t<T> binarytree_t<T>::set_root(const T& item)
{
	this->clear();
	this->_registry[0] = binarynode_t<T>(this, 0, &item);
	return iterator(*(this->_root));
}

template <typename T> inline void binarytree_t<T>::clear()
{
	this->_registry.clear();
	this->_rings = 0;
	this->_root = 0;
}

template <typename T> inline bool binaryiterator_t<T>::has_left() const
{
	return this->_node != 0 && this->_node->_left != 0;
}
template <typename T> inline bool binaryiterator_t<T>::has_right() const
{
	return this->_node != 0 && this->_node->_right != 0;
}
template <typename T> inline bool binaryiterator_t<T>::root() const
{
	return this->_node != 0 && this->_node->_parent == 0;
}
template <typename T> inline bool binaryiterator_t<T>::leaf() const
{
	return this->_node != 0 && this->_node->_left == 0 && this->_node->_right == 0;
}

template <typename T> inline binaryiterator_t<T>& binaryiterator_t<T>::operator++()
{
	if (this->_node != 0)
	{
		this->_node = this->_node->_left;
	}
	
	return *this;
}
template <typename T> inline binaryiterator_t<T>& binaryiterator_t<T>::operator--()
{
	if (this->_node != 0)
	{
		this->_node = this->_node->_right;
	}
	
	return *this;
}
template <typename T> inline T& binaryiterator_t<T>::operator*() const { return *(this->_node->_data); }
template <typename T> inline bool binaryiterator_t<T>::operator==(const binaryiterator_t<T>& other) const
{
	return this->_node == other._node;
}
template <typename T> inline bool binaryiterator_t<T>::operator!=(const binaryiterator_t<T>& other) const
{
	return this->_node != other._node;
}