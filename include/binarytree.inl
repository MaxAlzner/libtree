#pragma once

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
template <typename T> inline binaryiterator_t<T> binaryiterator_t<T>::parent()
{
    return this->_up != 0 ? binaryiterator_t<T>(this->_up) : binaryiterator_t<T>();
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