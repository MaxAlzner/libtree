#pragma once

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

#if !defined(min)
#define min(x, y) (x < y ? x : y)
#endif
#if !defined(max)
#define max(x, y) (x > y ? x : y)
#endif

static uint32_t tree_index_in_ring(const uint32_t index, const uint32_t stride)
{
	float e = 0.0f;
	uint32_t last = 0;
	while (index >= (last += (uint32_t)pow((float)stride, e)))
	{
		e += 1.0f;
	}
	
	return (uint32_t)e;
}

static size_t tree_size(const uint32_t rings, const uint32_t stride)
{
	size_t size = 0;
	for (uint32_t i = 0; i < rings; i++)
	{
		size += (size_t)pow((float)stride, i);
	}
	
	return size;
}

template <typename T> class treealloc_t
{
public:
	
	inline treealloc_t() :
		_buffer(0),
		_bytes(0),
		_rings(0),
		_stride(1) {}
	inline treealloc_t(const uint32_t rings, const uint32_t stride) :
		_buffer(0),
		_bytes(0),
		_rings(rings),
		_stride(max(stride, 1)) {}
	inline ~treealloc_t() {}
	
	inline void alloc(const uint32_t rings, const uint32_t stride);
	inline void ensure(const uint32_t rings, const uint32_t stride);
	
	inline void clear();
	inline void zero();
	
	inline size_t capacity() const { return this->_bytes / sizeof(T); }
	
	inline T& operator[](const size_t index);
	
protected:
	
	T* _buffer;
	size_t _bytes;
	uint32_t _rings;
	uint32_t _stride;
	
};

template <typename T> inline void treealloc_t<T>::alloc(const uint32_t rings, const uint32_t stride)
{
	size_t size = tree_size(rings, stride);
	size_t bytes = size * sizeof(T);
	T* clean = (T*)calloc(size, sizeof(T));
	if (this->_buffer != 0)
	{
		memcpy(clean, this->_buffer, min(bytes, this->_bytes));
	}
	
	this->clear();
	this->_buffer = clean;
	this->_bytes = bytes;
	this->_rings = rings;
	this->_stride = stride;
}
template <typename T> inline void treealloc_t<T>::ensure(const uint32_t rings, const uint32_t stride)
{
	if (this->_stride != stride)
	{
		this->clear();
	}
	
	if (rings > this->_rings || this->_bytes < 1)
	{
		this->_rings = max(this->_rings, rings);
		this->_stride = max(this->_stride, stride);
		this->alloc(this->_rings, this->_stride);
	}
}

template <typename T> inline void treealloc_t<T>::clear()
{
	if (this->_buffer != 0)
	{
		free(this->_buffer);
	}
	
	this->_buffer = 0;
	this->_bytes = 0;
}
template <typename T> inline void treealloc_t<T>::zero()
{
	memset(this->_buffer, 0, this->_bytes);
}

template <typename T> inline T& treealloc_t<T>::operator[](const size_t index)
{
	uint32_t ring = tree_index_in_ring(index, this->_stride);
	this->ensure(ring + 1, this->_stride);
	return *(this->_buffer + index);
}

template <typename T> struct binarynode_t;
template <typename T> struct binaryiterator_t;
template <typename T> class binarytree_t;

template <typename T> struct binarynode_t
{
	
	inline binarynode_t() :
		_left(0),
		_right(0),
		_up(0),
		_tree(0),
		_ring(-1),
		_branch(-1),
		_data(0) {}
	inline binarynode_t(binarytree_t<T>* tree, const int32_t ring, const int32_t branch, const T& data) :
		_left(0),
		_right(0),
		_up(0),
		_tree(tree),
		_ring(ring),
		_branch(branch),
		_data(data) {}
	inline ~binarynode_t() {}
	
	inline bool empty() const;
	
	binarynode_t<T>* _left;
	binarynode_t<T>* _right;
	binarynode_t<T>* _up;
	binarytree_t<T>* _tree;
	int32_t _ring;
	int32_t _branch;
	T _data;
	
};

template <typename T> struct binaryiterator_t
{
	
	inline binaryiterator_t() : _node(0) {}
	inline binaryiterator_t(const binarynode_t<T>& node) : _node((binarynode_t<T>*)&node) {}
	inline ~binaryiterator_t() {}
	
	inline binaryiterator_t<T> left() const;
	inline binaryiterator_t<T> left(const T& item);
	inline binaryiterator_t<T> right() const;
	inline binaryiterator_t<T> right(const T& item);
	
	inline bool has_left() const;
	inline bool has_right() const;
	inline bool root() const;
	inline bool leaf() const;
	
	inline bool empty() const;
	
	inline binaryiterator_t<T>& operator++();
	inline binaryiterator_t<T>& operator--();
	inline T& operator*() const;
	inline bool operator==(const binaryiterator_t<T>& other) const;
	inline bool operator!=(const binaryiterator_t<T>& other) const;
	
	binarynode_t<T>* _node;
	
};

template <typename T> class binarytree_t
{
public:
	
	typedef binaryiterator_t<T> iterator;
	
	inline binarytree_t() :
		_registry(3, 2),
		_root(0) {}
	inline ~binarytree_t() { this->clear(); }
	
	friend struct binaryiterator_t<T>;
	
	inline iterator set_root(const T& item);
	
	inline iterator root() const { return iterator(*(this->_root)); }
	inline iterator end() const { return iterator(); }
	inline void clear();
	
	inline int32_t nodes(binarynode_t<T>* buffer, size_t size);
	
protected:
	
	treealloc_t<binarynode_t<T> > _registry;
	binarynode_t<T>* _root;
	
};

template <typename T> inline bool binarynode_t<T>::empty() const
{
	return this->_data == 0 || this->_tree == 0 || this->_ring < 0 || this->_branch < 0;
}

template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::left() const
{
	return this->_node != 0 && this->_node->_left != 0 ? binaryiterator_t<T>(*(this->_node->_left)) : binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::left(const T& item)
{
	if (this->_node != 0 && this->_node->_tree != 0)
	{
		int32_t ring = this->_node->_ring + 1;
		int32_t branch = (this->_node->_branch * 2) + 1;
		int32_t index = (((int32_t)pow(2.0f, (float)ring)) - 1) + branch;
		binarynode_t<T>& node = (this->_node->_tree->_registry[index] = binarynode_t<T>(this->_node->_tree, ring, branch, item));
		node._up = this->_node;
		this->_node->_left = &node;
		return binaryiterator_t(node);
	}
	
	return binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::right() const
{
	return this->_node != 0 && this->_node->_right != 0 ? binaryiterator_t<T>(*(this->_node->_right)) : binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::right(const T& item)
{
	if (this->_node != 0)
	{
		int32_t ring = this->_node->_ring + 1;
		int32_t branch = this->_node->_branch * 2;
		int32_t index = (((int32_t)pow(2.0f, (float)ring)) - 1) + branch;
		binarynode_t<T>& node = (this->_node->_tree->_registry[index] = binarynode_t<T>(this->_node->_tree, ring, branch, item));
		node._up = this->_node;
		this->_node->_right = &node;
		return binaryiterator_t(node);
	}
	
	return binaryiterator_t<T>();
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

template <typename T> inline bool binaryiterator_t<T>::empty() const
{
	return this->_node == 0;
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
template <typename T> inline T& binaryiterator_t<T>::operator*() const
{
	return *(this->_node->_data);
}
template <typename T> inline bool binaryiterator_t<T>::operator==(const binaryiterator_t<T>& other) const
{
	return this->_node == other._node;
}
template <typename T> inline bool binaryiterator_t<T>::operator!=(const binaryiterator_t<T>& other) const
{
	return this->_node != other._node;
}

template <typename T> inline binaryiterator_t<T> binarytree_t<T>::set_root(const T& item)
{
	this->_registry.zero();
	this->_root = &(this->_registry[0] = binarynode_t<T>(this, 0, 0, item));
	return iterator(*(this->_root));
}

template <typename T> inline void binarytree_t<T>::clear()
{
	this->_registry.clear();
	this->_root = 0;
}

template <typename T> inline int32_t binarytree_t<T>::nodes(binarynode_t<T>* buffer, size_t size)
{
	binarynode_t<T>* read = buffer;
	int32_t count = 0;
	for (size_t i = 0; i < this->_registry.capacity(); i++)
	{
		const binarynode_t<T>* node = &(this->_registry[i]);
		if (!node->empty())
		{
			*read = *node;
			read++;
			count++;
			if (count >= size)
			{
				break;
			}
		}
	}
	
	return count;
}