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
/// Calculates the tree ring for the given index and stride.
/// </summary>
/// <param name="index">The index inside of the tree.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>The ring's index.</returns>
inline uint32_t tree_ring_by_index(const size_t index, const uint32_t stride);

/// <summary>
/// Calculates the tree ring branch for the given index and stride.
/// </summary>
/// <param name="index">The index inside of the tree.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>The branch's index.</returns>
inline uint32_t tree_branch_by_index(const size_t index, const uint32_t stride);

/// <summary>
/// Calculates the size needed for the given number of rings and specified stride.
/// </summary>
/// <param name="rings">The number of rings that make up the tree.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>The number of elements that the tree can potentially hold.</returns>
inline size_t tree_size(const uint32_t rings, const uint32_t stride);

/// <summary>
/// Calculates the size of a ring with the specified stride.
/// </summary>
/// <param name="ring">The index of a tree ring.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>The number of elements that a tree ring can potentially hold.</returns>
inline size_t tree_ring_length(const uint32_t ring, const uint32_t stride);

/// <summary>
/// Calculates a tree index.
/// </summary>
/// <param name="ring">The ring that the node exists in.</param>
/// <param name="branch">The index inside of the ring for the node.</param>
/// <param name="stride">The number of child nodes for each parent.</param>
/// <returns>An index that could exists in the tree.</returns>
inline size_t tree_index(const uint32_t ring, const uint32_t branch, const uint32_t stride);

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
	/// Remove the entire node chain starting at the given root.
	/// </summary>
	/// <param name="index">The index of the root node to delete.</param>
	inline void remove(const size_t index);
	
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

#include "treealloc.inl"

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
		_branch(-1) {}
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
	
	/// <summary>
	/// Gets the index inside of the tree the node.
	/// </summary>
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
	/// <param name="node">The current node for the iterator.</param>
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
	/// Iterate to the parent node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> parent();
	
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
	/// Gets a value indicating whether or not the node is a root node.
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

#include "binarytree.inl"

template <typename T> struct quadnode_t;
template <typename T> struct quaditerator_t;
template <typename T> class quadtree_t;

/// <summary>
/// Contains methods and properties for a node in a quadratic tree.
/// </summary>
template <typename T> struct quadnode_t
{
	
	inline quadnode_t() :
		_tree(0),
		_ring(-1),
		_branch(-1) {}
	/// <param name="tree">Pointer to the node's tree.</param>
	/// <param name="ring">The ring that the node exists in.</param>
	/// <param name="branch">The index inside of the ring for the node.</param>
	/// <param name="data">The data that the node holds.</param>
	inline quadnode_t(const quadtree_t<T>& tree, const int32_t ring, const int32_t branch, const T& data) :
		_q0(tree._registry),
		_q1(tree._registry),
		_q2(tree._registry),
		_q3(tree._registry),
		_up(tree._registry),
		_tree((quadtree_t<T>*)&tree),
		_ring(ring),
		_branch(branch),
		_data(data) {}
	inline ~quadnode_t() {}
	
	/// <summary>
	/// Gets a values indicating whether the node is empty.
	/// </summary>
	inline bool empty() const;
	
	/// <summary>
	/// Gets the index inside of the tree the node.
	/// </summary>
	inline size_t index() const;
	
	treereference_t<quadnode_t<T> > _q0;
	treereference_t<quadnode_t<T> > _q1;
	treereference_t<quadnode_t<T> > _q2;
	treereference_t<quadnode_t<T> > _q3;
	treereference_t<quadnode_t<T> > _up;
	quadtree_t<T>* _tree;
	int32_t _ring;
	int32_t _branch;
	T _data;
	
};

/// <summary>
/// Contains methods and properties for iterating through a quadratic tree.
/// </summary>
template <typename T> struct quaditerator_t
{
	
	inline quaditerator_t() {}
	/// <param name="node">The current node for the iterator.</param>
	inline quaditerator_t(const treereference_t<quadnode_t<T> >& node) :
		_node(node) {}
	inline ~quaditerator_t() {}
	
	/// <summary>
	/// Iterates to a child node at the specified number.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline quaditerator_t<T> child(int32_t num);
	/// <summary>
	/// Iterate to the parent node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline binaryiterator_t<T> parent();
	
	/// <summary>
	/// Remove the node where the iterator is, and then iterate to the parent node.
	/// </summary>
	/// <returns>A new iterator at the next position.</returns>
	inline quaditerator_t<T> remove();
	
	/// <summary>
	/// Gets a value indicating whether or not the node is a root node.
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
	/// Gets the held item for the current node.
	/// </summary>
	inline T& operator*() const;
	/// <summary>
	/// Determines whether this iterator is at the same location than the other iterator.
	/// </summary>
	/// <param name="other">An instance of binaryiterator_t.</param>
	inline bool operator==(const quaditerator_t<T>& other) const;
	/// <summary>
	/// Determines whether this iterator is not at the same location than the other iterator.
	/// </summary>
	/// <param name="other">An instance of binaryiterator_t.</param>
	inline bool operator!=(const quaditerator_t<T>& other) const;
	
	treereference_t<quadnode_t<T> > _node;
	
};

/// <summary>
/// Contains methods and properties for a quadratic tree.
/// </summary>
template <typename T> class quadtree_t
{
public:
	
	typedef quaditerator_t<T> iterator;
	
	typedef int32_t (*iterationfunc)(const treereference_t<quadnode_t<T> >& node, const T& item);
	
	inline quadtree_t() :
		_registry(3, 4) {}
	/// <param name="rings">The number of rings that make up the tree.</param>
	inline quadtree_t(const uint32_t rings) :
		_registry(rings, 4) {}
	inline ~quadtree_t() { this->clear(); }
	
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
	inline iterator root() { return iterator(treereference_t<quadnode_t<T> >(this->_registry, 0)); }
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
	/// The return value must correspond to the next quadrant in the path (1, 2, 3, 4), and zero will exit.
	/// </summary>
	/// <param name="callback">Callback function to call on each node in the path.</param>
	inline void path(iterationfunc callback);
	
	/// <summary>
	/// Clears all nodes from the tree.
	/// </summary>
	inline void clear();
	
protected:
	
	treealloc_t<quadnode_t<T> > _registry;
	
};

#include "quadtree.inl"