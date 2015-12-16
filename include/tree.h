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

/// <summary>
/// Gets the tree ring for the given index and stride.
/// </summary>
/// <param name="index">The index inside of the tree.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>The ring's index.</returns>
static uint32_t tree_ring_by_index(const uint32_t index, const uint32_t stride)
{
	float e = 0.0f;
	uint32_t last = 0;
	while (index >= (last += (uint32_t)pow((float)stride, e)))
	{
		e += 1.0f;
	}
	
	return (uint32_t)e;
}

/// <summary>
/// Gets the size needed for the given number of rings and specified stride.
/// </summary>
/// <param name="rings The number of rings that make up the tree.</param>
/// <param name="stride The number of child nodes for each parent.</param>
/// <returns>The number of elements that the tree can potentially hold.</returns>
static size_t tree_size(const uint32_t rings, const uint32_t stride)
{
	size_t size = 0;
	for (uint32_t i = 0; i < rings; i++)
	{
		size += (size_t)pow((float)stride, i);
	}
	
	return size;
}

/// <summary>
/// Contains methods and properties for allocating and indexing a tree buffer.
/// </summary>
template <typename T> class treealloc_t
{
public:
	
	inline treealloc_t() :
		_buffer(0),
		_bytes(0),
		_rings(0),
		_stride(1) {}
	/// <param name="rings">The number of rings that make up the tree.</param>
	/// <param name="stride">The number of child nodes for each parent.</param>
	inline treealloc_t(const uint32_t rings, const uint32_t stride) :
		_buffer(0),
		_bytes(0),
		_rings(rings),
		_stride(max(stride, 1)) {}
	inline ~treealloc_t() {}
	
	/// <summary>
	/// Allocates creates or re-allocates a the tree buffer.
	/// </summary>
	/// <param name="rings">The number of rings that make up the tree.</param>
	/// <param name="stride">The number of child nodes for each parent.</param>
	inline void alloc(const uint32_t rings, const uint32_t stride);
	/// <summary>
	/// Ensures that the tree buffer has atleast the specified number of rings and stride.
	/// </summary>
	/// <param name="rings">The number of rings that make up the tree.</param>
	/// <param name="stride">The number of child nodes for each parent.</param>
	inline void ensure(const uint32_t rings, const uint32_t stride);
	
	/// <summary>
	/// Frees the tree buffer.
	/// </summary>
	inline void clear();
	/// <summary>
	/// Sets the entire tree buffer to null.
	/// </summary>
	inline void zero();
	
	/// <summary>
	/// Gets the total capacity of the tree buffer.
	/// </summary>
	inline size_t capacity() const { return this->_bytes / sizeof(T); }
	
	/// <summary>
	/// Indexes into the tree buffer for an element.
	/// </summary>
	inline T& operator[](const size_t index);
	
protected:
	
	T* _buffer;
	size_t _bytes;
	uint32_t _rings;
	uint32_t _stride;
	
};

/// <summary>
/// Contains methods and properties for refrencing an element in a treealloc_t instance.
/// </summary>
template <typename T> class treereference_t
{
public:
	
	inline treereference_t() :
		_index(-1),
		_registry(0) {}
	/// <param name="registry">An instance of treealloc_t to reference.</param>
	inline treereference_t(const treealloc_t<T>& registry) :
		_index(-1),
		_registry((treealloc_t<T>*)&registry) {}
	/// <param name="registry">An instance of treealloc_t to reference.</param>
	/// <param name="index">The index to reference.</param>
	inline treereference_t(const treealloc_t<T>& registry, const uint32_t index) :
		_index(index),
		_registry((treealloc_t<T>*)&registry) {}
	inline ~treereference_t() {}
	
	/// <summary>
	/// Gets a value indicating whether or not the reference is empty.
	/// </summary>
	inline bool empty() const;
	
	/// <summary>
	/// Sets the reference to the specified index.
	/// </summary>
	/// <param name="index">An index to reference.</param>
	inline treereference_t<T>& operator=(const int32_t index);
	/// <summary>
	/// Sets this instance to be equal to that instance.
	/// </summary>
	/// <param name="other">An instance of treereference_t.</param>
	inline treereference_t<T>& operator=(const treereference_t<T>& other);
	/// <summary>
	/// Accesses the referenced element.
	/// </summary>
	inline T& operator*() const;
	/// <summary>
	/// Accesses a member from the referenced element.
	/// </summary>
	inline T* operator->() const;
	/// <summary>
	/// Converts this instance into a point to the referenced element.
	/// </summary>
	inline operator T*() const;
	
protected:
	
	int32_t _index;
	treealloc_t<T>* _registry;
	
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
	uint32_t ring = tree_ring_by_index(index, this->_stride);
	this->ensure(ring + 1, this->_stride);
	return *(this->_buffer + index);
}

template <typename T> inline bool treereference_t<T>::empty() const
{
	return this->_registry == 0 || this->_index < 0;
}

template <typename T> inline treereference_t<T>& treereference_t<T>::operator=(const int32_t index)
{
	this->_index = index;
	return *this;
}
template <typename T> inline treereference_t<T>& treereference_t<T>::operator=(const treereference_t<T>& other)
{
	this->_index = other._index;
	this->_registry = other._registry;
	return *this;
}
template <typename T> inline T& treereference_t<T>::operator*() const
{
	// if (this->_registry != 0 && this->_index >= 0)
	// {
		return (*(this->_registry))[this->_index];
	// }
}
template <typename T> inline T* treereference_t<T>::operator->() const
{
	if (this->_registry != 0 && this->_index >= 0)
	{
		return &((*(this->_registry))[this->_index]);
	}
	
	return 0;
}
template <typename T> inline treereference_t<T>::operator T*() const
{
	if (this->_registry != 0 && this->_index >= 0)
	{
		return &((*(this->_registry))[this->_index]);
	}
	
	return 0;
}

template <typename T> struct binarynode_t;
template <typename T> struct binaryiterator_t;
template <typename T> class binarytree_t;

/// <summary>
/// Contains methods and properties for a node in a binary tree.
/// </summary>
template <typename T> struct binarynode_t
{
	
	inline binarynode_t() :
		_tree(0),
		_ring(-1),
		_branch(-1),
		_data(0) {}
	/// <param name="tree">Pointer to the node's tree.</param>
	/// <param name="ring">The ring that the node exists in.</param>
	/// <param name="branch">The index inside of the ring for the node.</param>
	/// <param name="data">The data that the node holds.</param>
	inline binarynode_t(const binarytree_t<T>& tree, const int32_t ring, const int32_t branch, const T& data) :
		_left(tree._registry),
		_right(tree._registry),
		_up(tree._registry),
		_tree((binarytree_t<T>*)&tree),
		_ring(ring),
		_branch(branch),
		_data(data) {}
	inline ~binarynode_t() {}
	
	/// <summary>
	/// Gets a values indicating whether the node is empty.
	/// </summary>
	inline bool empty() const;
	
	inline size_t index() const;
	
	treereference_t<binarynode_t<T> > _left;
	treereference_t<binarynode_t<T> > _right;
	treereference_t<binarynode_t<T> > _up;
	binarytree_t<T>* _tree;
	int32_t _ring;
	int32_t _branch;
	T _data;
	
};

/// <summary>
/// Contains methods and properties for iterating through a binary tree.
/// </summary>
template <typename T> struct binaryiterator_t
{
	
	inline binaryiterator_t() {}
	/// <param name="node">The current node for the iterator.
	inline binaryiterator_t(const treereference_t<binarynode_t<T> >& node) :
		_node(node) {}
	inline ~binaryiterator_t() {}
	
	/// <summary>
	/// Iterate to the left child node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> left() const;
	/// <summary>
	/// Set the left child node with the given item, and then iterate to that node.
	/// </summary>
	/// <param name="item">The item to put in the new node.</param>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> left(const T& item);
	/// <summary>
	/// Iterate to the right child node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> right() const;
	/// <summary>
	/// Set the right child node with the given item, and then iterate to that node.
	/// </summary>
	/// <param name="item">The item to put in the new node.</param>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> right(const T& item);
	
	/// <summary>
	/// Remove the node where the iterator is, and then iterate to the parent node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> remove();
	
	/// <summary>
	/// Gets a value indicating whether or not the node has a left child.
	/// </summary>
	inline bool has_left() const;
	/// <summary>
	/// Gets a value indicating whether or not the node has a right child.
	/// </summary>
	inline bool has_right() const;
	/// <summary>
	/// Gets a value indicating whether or not the node is at the root of the tree.
	/// </summary>
	inline bool root() const;
	/// <summary>
	/// Gets a value indicating whether or not the node has any children.
	/// </summary>
	inline bool leaf() const;
	
	/// <summary>
	/// Gets a value indicating whether or not the iterator has a node.
	/// </summary>
	inline bool empty() const;
	
	/// <summary>
	/// Iterate to the left child node.
	/// </summary>
	inline binaryiterator_t<T>& operator++();
	/// <summary>
	/// Iterate to the right child node.
	/// </summary>
	inline binaryiterator_t<T>& operator--();
	/// <summary>
	/// Gets the held item for the current node.
	/// </summary>
	inline T& operator*() const;
	/// <summary>
	/// Determines whether this iterator is at the same location than the other iterator.
	/// </summary>
	/// <param name="other">An instance of binaryiterator_t.</param>
	inline bool operator==(const binaryiterator_t<T>& other) const;
	/// <summary>
	/// Determines whether this iterator is not at the same location than the other iterator.
	/// </summary>
	/// <param name="other">An instance of binaryiterator_t.</param>
	inline bool operator!=(const binaryiterator_t<T>& other) const;
	
	treereference_t<binarynode_t<T> > _node;
	
};

/// <summary>
/// Contains methods and properties for a binary tree.
/// </summary>
template <typename T> class binarytree_t
{
public:
	
	typedef binaryiterator_t<T> iterator;
	
	typedef int32_t (*iterationfunc)(const treereference_t<binarynode_t<T> >& node, const T& item);
	
	friend struct binarynode_t<T>;
	friend struct binaryiterator_t<T>;
	
	inline binarytree_t() :
		_registry(3, 2) {}
	/// <param name="rings">The number of rings that make up the tree.</param>
	inline binarytree_t(const uint32_t rings) :
		_registry(rings, 2) {}
	inline ~binarytree_t() { this->clear(); }
	
	/// <summary>
	/// Sets the root of the tree with the given item.
	/// </summary>
	/// <param name="item">An item to put in the root node.</param>
	/// <returns>An iterator pointing at the root of the tree.</returns>
	inline iterator set_root(const T& item);
	
	/// <summary>
	/// Searches the tree for the given item.
	/// </summary>
	/// <param name="item">An item to search for.</param>
	/// <reutrns>A value indicating whether or not the given item was found in the tree.</returns>
	inline iterator search(const T& item);
	
	/// <summary>
	/// Gets an iterator pointing at the root of the tree.
	/// </summary>
	inline iterator root() { return iterator(treereference_t<binarynode_t<T> >(this->_registry, 0)); }
	/// <summary>
	/// Gets an invalid iterator that does not have a node.
	/// </summary>
	inline iterator end() const { return iterator(); }
	
	/// <summary>
	/// Call given function for each node in the tree.
	/// A zero value as a return value will exit.
	/// </summary>
	/// <param name="callback">Callback function to call on each node in the tree.</param>
	inline void each(iterationfunc callback);
	/// <summary>
	/// Call to iterate through tree determined by the return value of the callback.
	/// A positive value will go left, a negative value will go right, and zero will exit.
	/// </summary>
	/// <param name="callback">Callback function to call on each node in the path.</param>
	inline void path(iterationfunc callback);
	
	/// <summary>
	/// Clears all nodes from the tree.
	/// </summary>
	inline void clear();
	
protected:
	
	static int32_t execute_each(const treereference_t<binarynode_t<T> >& node, iterationfunc callback);
	static int32_t execute_path(const treereference_t<binarynode_t<T> >& node, iterationfunc callback);
	
	treealloc_t<binarynode_t<T> > _registry;
	
};

template <typename T> inline bool binarynode_t<T>::empty() const
{
	return this->_data == 0 || this->_tree == 0 || this->_ring < 0 || this->_branch < 0;
}

template <typename T> inline size_t binarynode_t<T>::index() const
{
	return (size_t)((((uint32_t)pow(2.0f, (float)this->_ring)) - 1) + this->_branch);
}

template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::left() const
{
	return this->_node != 0 && this->_node->_left != 0 ? binaryiterator_t<T>(this->_node->_left) : binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::left(const T& item)
{
	if (this->_node != 0 && this->_node->_tree != 0)
	{
		int32_t ring = this->_node->_ring + 1;
		int32_t branch = (this->_node->_branch * 2) + 1;
		int32_t index = (((int32_t)pow(2.0f, (float)ring)) - 1) + branch;
		binarynode_t<T>& node = (this->_node->_tree->_registry[index] = binarynode_t<T>(*(this->_node->_tree), ring, branch, item));
		node._up = this->_node;
		this->_node->_left = treereference_t<binarynode_t<T> >(this->_node->_tree->_registry, index);
		return binaryiterator_t<T>(this->_node->_left);
	}
	
	return binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::right() const
{
	return this->_node != 0 && this->_node->_right != 0 ? binaryiterator_t<T>(this->_node->_right) : binaryiterator_t<T>();
}
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::right(const T& item)
{
	if (this->_node != 0)
	{
		int32_t ring = this->_node->_ring + 1;
		int32_t branch = this->_node->_branch * 2;
		int32_t index = (((int32_t)pow(2.0f, (float)ring)) - 1) + branch;
		binarynode_t<T>& node = (this->_node->_tree->_registry[index] = binarynode_t<T>(*(this->_node->_tree), ring, branch, item));
		node._up = this->_node;
		this->_node->_right = treereference_t<binarynode_t<T> >(this->_node->_tree->_registry, index);
		return binaryiterator_t<T>(this->_node->_right);
	}
	
	return binaryiterator_t<T>();
}

template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::remove()
{
	if (this->_node != 0)
	{
		treereference_t<binarynode_t<T> > prev = this->_node;
		if (prev->_up != 0)
		{
			if (prev->_up->_left == prev) { prev->_up->_left = -1; }
			else if (prev->_up->_right == prev) { prev->_up->_right = -1; }
		}
		
		this->_node = prev->_up;
		*prev = binarynode_t<T>();
		if (this->_node != 0)
		{
			return binaryiterator_t<T>(this->_node);
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
	this->_registry[0] = binarynode_t<T>(*this, 0, 0, item);
	return iterator(treereference_t<binarynode_t<T> >(this->_registry, 0));
}

template <typename T> inline binaryiterator_t<T> binarytree_t<T>::search(const T& item)
{
	for (size_t i = 0; i < this->_registry.capacity(); i++)
	{
		treereference_t<binarynode_t<T> > node(this->_registry, i);
		if (!node.empty() && !node->empty() && memcmp(&(node->_data), &item, sizeof(T)) == 0)
		{
			return binaryiterator_t<T>(node);
		}
	}
	
	return binaryiterator_t<T>();
}

template <typename T> inline void binarytree_t<T>::each(iterationfunc callback)
{
	execute_each(treereference_t<binarynode_t<T> >(this->_registry, 0), callback);
}
template <typename T> inline void binarytree_t<T>::path(iterationfunc callback)
{
	execute_path(treereference_t<binarynode_t<T> >(this->_registry, 0), callback);
}

template <typename T> inline void binarytree_t<T>::clear()
{
	this->_registry.clear();
}

template <typename T> int32_t binarytree_t<T>::execute_each(const treereference_t<binarynode_t<T> >& node, iterationfunc callback)
{
	int32_t result = 1;
	if (node != 0 && !node.empty() && !node->empty() && callback != 0)
	{
		result = callback(node, node->_data);
		if (result != 0)
		{
			result = execute_each(node->_left, callback);
			if (result != 0)
			{
				result = execute_each(node->_right, callback);
			}
		}
	}
	
	return result;
}
template <typename T> int32_t binarytree_t<T>::execute_path(const treereference_t<binarynode_t<T> >& node, iterationfunc callback)
{
	int32_t result = 0;
	if (node != 0 && !node.empty() && !node->empty() && callback != 0)
	{
		result = callback(node, node->_data);
		if (result != 0)
		{
			if (result > 0)
			{
				return execute_path(node->_left, callback);
			}
			else
			{
				return execute_path(node->_right, callback);
			}
		}
	}
	
	return result;
}