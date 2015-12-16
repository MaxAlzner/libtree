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