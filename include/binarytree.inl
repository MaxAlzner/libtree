#pragma once

inline uint32_t tree_ring_by_index(const size_t index, const uint32_t stride)
{
	float e = 0.0f;
	size_t last = 0;
	while (index >= (last += (size_t)pow((float)stride, e)))
	{
		e += 1.0f;
	}
	
	return (uint32_t)e;
}

inline uint32_t tree_branch_by_index(const size_t index, const uint32_t stride)
{
	uint32_t i = index + 1;
	float e = 0.0f;
	size_t last = 0;
	while (i > (last = (size_t)pow((float)stride, e)))
	{
		i -= last;
		e += 1.0f;
	}
	
	return i - 1;
}

inline size_t tree_size(const uint32_t rings, const uint32_t stride)
{
	size_t size = 0;
	for (uint32_t i = 0; i < rings; i++)
	{
		size += (size_t)pow((float)stride, i);
	}
	
	return size;
}

inline size_t tree_ring_length(const uint32_t ring, const uint32_t stride)
{
	return (size_t)pow((float)stride, (float)ring);
}

inline size_t tree_index(const uint32_t ring, const uint32_t branch, const uint32_t stride)
{
	return (tree_ring_length(ring, stride) - 1) + (size_t)branch;
}

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

template <typename T> inline void treealloc_t<T>::remove(const size_t index)
{
	uint32_t ring = tree_ring_by_index(index, this->_stride);
	size_t root = index % this->capacity();
	uint32_t branch = tree_branch_by_index(root, this->_stride);
	for (uint32_t i = ring; i < this->_rings; i++)
	{
		memset(this->_buffer + tree_index(i, branch, this->_stride), 0, sizeof(T) * tree_ring_length(i - ring, this->_stride));
		branch = branch * this->_stride;
	}
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
		prev->_tree->_registry.remove(prev->index());
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