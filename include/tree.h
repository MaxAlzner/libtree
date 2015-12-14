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

/**
 * Gets the tree ring for the given index and stride.
 * @param index The index inside of the tree.
 * @param stride The number of child nodes for each parent.
 * @return The ring's index.
 */
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

/**
 * Gets the size needed for the given number of rings and specified stride.
 * @param rings The number of rings that make up the tree.
 * @param stride The number of child nodes for each parent.
 * @return The number of elements that the tree can potentially hold.
 */
static size_t tree_size(const uint32_t rings, const uint32_t stride)
{
	size_t size = 0;
	for (uint32_t i = 0; i < rings; i++)
	{
		size += (size_t)pow((float)stride, i);
	}
	
	return size;
}

/**
 * Contains methods and properties for allocating and indexing a tree buffer.
 */
template <typename T> class treealloc_t
{
public:
	
	inline treealloc_t() :
		_buffer(0),
		_bytes(0),
		_rings(0),
		_stride(1) {}
	/**
	 * @param rings The number of rings that make up the tree.
	 * @param stride The number of child nodes for each parent.
	 */
	inline treealloc_t(const uint32_t rings, const uint32_t stride) :
		_buffer(0),
		_bytes(0),
		_rings(rings),
		_stride(max(stride, 1)) {}
	inline ~treealloc_t() {}
	
	/**
	 * Allocates creates or re-allocates a the tree buffer.
	 * @param rings The number of rings that make up the tree.
	 * @param stride The number of child nodes for each parent.
	 */
	inline void alloc(const uint32_t rings, const uint32_t stride);
	/**
	 * Ensures that the tree buffer has atleast the specified number of rings and stride.
	 * @param rings The number of rings that make up the tree.
	 * @param stride The number of child nodes for each parent.
	 */
	inline void ensure(const uint32_t rings, const uint32_t stride);
	
	/**
	 * Frees the tree buffer.
	 */
	inline void clear();
	/**
	 * Sets the entire tree buffer to null.
	 */
	inline void zero();
	
	/**
	 * Gets the total capacity of the tree buffer.
	 */
	inline size_t capacity() const { return this->_bytes / sizeof(T); }
	
	/**
	 * Indexes into the tree buffer for an element.
	 */
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

/**
 * Contains methods and properties for a node in a binary tree.
 */
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
	/**
	 * @param tree Pointer to the node's tree.
	 * @param ring The ring that the node exists in.
	 * @param branch The index inside of the ring for the node.
	 * @param data The data that the node holds.
	 */
	inline binarynode_t(binarytree_t<T>* tree, const int32_t ring, const int32_t branch, const T& data) :
		_left(0),
		_right(0),
		_up(0),
		_tree(tree),
		_ring(ring),
		_branch(branch),
		_data(data) {}
	inline ~binarynode_t() {}
	
	/**
	 * Gets a values indicating whether the node is empty.
	 */
	inline bool empty() const;
	
	binarynode_t<T>* _left;
	binarynode_t<T>* _right;
	binarynode_t<T>* _up;
	binarytree_t<T>* _tree;
	int32_t _ring;
	int32_t _branch;
	T _data;
	
};

/**
 * Contains methods and properties for iterating through a binary tree.
 */
template <typename T> struct binaryiterator_t
{
	
	inline binaryiterator_t() : _node(0) {}
	/**
	 * @param node The current node for the iterator.
	 */
	inline binaryiterator_t(const binarynode_t<T>& node) : _node((binarynode_t<T>*)&node) {}
	inline ~binaryiterator_t() {}
	
	/**
	 * Iterate to the left child node.
	 * @return A new iterator at the next position.
	 */
	inline binaryiterator_t<T> left() const;
	/**
	 * Set the left child node with the given item, and then iterate to that node.
	 * @param item The item to put in the new node.
	 * @return A new iterator at the next position.
	 */
	inline binaryiterator_t<T> left(const T& item);
	/**
	 * Iterate to the right child node.
	 * @return A new iterator at the next position.
	 */
	inline binaryiterator_t<T> right() const;
	/**
	 * Set the right child node with the given item, and then iterate to that node.
	 * @param item The item to put in the new node.
	 * @return A new iterator at the next position.
	 */
	inline binaryiterator_t<T> right(const T& item);
	
	/**
	 * Remove the node where the iterator is, and then iterate to the parent node.
	 * @return A new iterator at the next position.
	 */
	inline binaryiterator_t<T> remove();
	
	/**
	 * Gets a value indicating whether or not the node has a left child.
	 */
	inline bool has_left() const;
	/**
	 * Gets a value indicating whether or not the node has a right child.
	 */
	inline bool has_right() const;
	/**
	 * Gets a value indicating whether or not the node is at the root of the tree.
	 */
	inline bool root() const;
	/**
	 * Gets a value indicating whether or not the node has any children.
	 */
	inline bool leaf() const;
	
	/**
	 * Gets a value indicating whether or not the iterator has a node.
	 */
	inline bool empty() const;
	
	/**
	 * Iterate to the left child node.
	 */
	inline binaryiterator_t<T>& operator++();
	/**
	 * Iterate to the right child node.
	 */
	inline binaryiterator_t<T>& operator--();
	/**
	 * Gets the held item for the current node.
	 */
	inline T& operator*() const;
	/**
	 * Determines whether this iterator is at the same location than the other iterator.
	 */
	inline bool operator==(const binaryiterator_t<T>& other) const;
	/**
	 * Determines whether this iterator is not at the same location than the other iterator.
	 */
	inline bool operator!=(const binaryiterator_t<T>& other) const;
	
	binarynode_t<T>* _node;
	
};

template <typename T> class binarytree_t
{
public:
	
	typedef binaryiterator_t<T> iterator;
	
	friend struct binaryiterator_t<T>;
	
	inline binarytree_t() :
		_registry(3, 2),
		_root(0) {}
	/**
	 * @param rings The number of rings that make up the tree.
	 */
	inline binarytree_t(const uint32_t rings) :
		_registry(rings, 2),
		_root(0) {}
	inline ~binarytree_t() { this->clear(); }
	
	/**
	 * Sets the root of the tree with the given item.
	 * @return An iterator pointing at the root of the tree.
	 */
	inline iterator set_root(const T& item);
	
	/**
	 * Gets an iterator pointing at the root of the tree.
	 */
	inline iterator root() const { return iterator(*(this->_root)); }
	/**
	 * Gets an invalid iterator that does not have a node.
	 */
	inline iterator end() const { return iterator(); }
	
	/**
	 * Clears all nodes from the tree.
	 */
	inline void clear();
	
	/**
	 * Fills a buffer with all nodes in the tree.
	 * @param buffer A buffer of nodes.
	 * @param size The size of the buffer.
	 * @return The number of items put in the buffer.
	 */
	inline int32_t nodes(binarynode_t<T>* buffer, const size_t size);
	
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
		return binaryiterator_t<T>(node);
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
		return binaryiterator_t<T>(node);
	}
	
	return binaryiterator_t<T>();
}

template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::remove()
{
	if (this->_node != 0)
	{
		binarynode_t<T>* prev = this->_node;
		if (prev->_up != 0)
		{
			if (prev->_up->_left == prev) { prev->_up->_left = 0; }
			else if (prev->_up->_right == prev) { prev->_up->_right = 0; }
		}
		
		this->_node = prev->_up;
		*prev = binarynode_t<T>();
		if (this->_node != 0)
		{
			return binaryiterator_t<T>(*(this->_node));
		}
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

template <typename T> inline int32_t binarytree_t<T>::nodes(binarynode_t<T>* buffer, const size_t size)
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