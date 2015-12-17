#pragma once

template <typename T> inline bool quadnode_t<T>::empty() const
{
	return this->_tree == 0 || this->_ring < 0 || this->_branch < 0;
}

template <typename T> inline size_t quadnode_t<T>::index() const
{
	return tree_index(this->_ring, this->_branch, 4);
}

template <typename T> inline quaditerator_t<T> quaditerator_t<T>::child(const int32_t quadrant)
{
	if (this->_node != 0)
	{
		switch (quadrant)
		{
		case 0:
			return quaditerator_t<T>(this->_node->_q0);
		case 1:
			return quaditerator_t<T>(this->_node->_q1);
		case 2:
			return quaditerator_t<T>(this->_node->_q2);
		case 3:
			return quaditerator_t<T>(this->_node->_q3);
		default:
			break;
		}
	}
	
    return quaditerator_t<T>();
}
template <typename T> inline quaditerator_t<T> quaditerator_t<T>::parent()
{
    return this->_node != 0 && this->_node->_up != 0 ? quaditerator_t<T>(this->_node->_up) : quaditerator_t<T>();
}

template <typename T> inline quaditerator_t<T> quaditerator_t<T>::remove()
{
	if (this->_node != 0)
	{
		treereference_t<quadnode_t<T> > prev = this->_node;
		if (prev->_up != 0)
		{
			if (prev->_up->_q0 == prev) { prev->_up->_q0 = -1; }
			else if (prev->_up->_q1 == prev) { prev->_up->_q1 = -1; }
			else if (prev->_up->_q2 == prev) { prev->_up->_q2 = -1; }
			else if (prev->_up->_q3 == prev) { prev->_up->_q3 = -1; }
		}
		
		this->_node = prev->_up;
		prev->_tree->_registry.remove(prev->index());
		if (this->_node != 0)
		{
			return quaditerator_t<T>(this->_node);
		}
	}
	
	return quaditerator_t<T>();
}

template <typename T> inline bool quaditerator_t<T>::root() const
{
	return this->_node != 0 && this->_node->_parent == 0;
}
template <typename T> inline bool quaditerator_t<T>::leaf() const
{
	return this->_node != 0 && this->_node->_left == 0 && this->_node->_right == 0;
}

template <typename T> inline bool quaditerator_t<T>::empty() const
{
	return this->_node == 0;
}

template <typename T> inline T& quaditerator_t<T>::operator*() const
{
	return *(this->_node->_data);
}
template <typename T> inline quaditerator_t<T> quaditerator_t<T>::operator[](const int32_t quadrant)
{
    return this->child(quadrant);
}
template <typename T> inline bool quaditerator_t<T>::operator==(const quaditerator_t<T>& other) const
{
	return this->_node == other._node;
}
template <typename T> inline bool quaditerator_t<T>::operator!=(const quaditerator_t<T>& other) const
{
	return this->_node != other._node;
}

#include <stdio.h>

template <typename T> inline quaditerator_t<T> quadtree_t<T>::set_root(const T& item)
{
	this->_registry.zero();
	this->_registry[0] = quadnode_t<T>(*this, 0, 0, item);
	return quaditerator_t<T>(treereference_t<quadnode_t<T> >(this->_registry, 0));
}

template <typename T> inline quaditerator_t<T> quadtree_t<T>::search(const T& item)
{
	for (size_t i = 0; i < this->_registry.capacity(); i++)
	{
		treereference_t<quadnode_t<T> > node(this->_registry, i);
		if (!node.empty() && !node->empty() && memcmp(&(node->_data), &item, sizeof(T)) == 0)
		{
			return quaditerator_t<T>(node);
		}
	}
	
	return quaditerator_t<T>();
}

template <typename T> inline quaditerator_t<T> quadtree_t<T>::root()
{
	return quaditerator_t<T>(treereference_t<quadnode_t<T> >(this->_registry, 0));
}
template <typename T> inline quaditerator_t<T> quadtree_t<T>::end() const
{
	return quaditerator_t<T>();
}

template <typename T> inline void quadtree_t<T>::each(iterationfunc callback)
{
	execute_each(treereference_t<quadnode_t<T> >(this->_registry, 0), callback);
}
template <typename T> inline void quadtree_t<T>::path(iterationfunc callback)
{
	execute_path(treereference_t<quadnode_t<T> >(this->_registry, 0), callback);
}

template <typename T> inline void quadtree_t<T>::clear()
{
	this->_registry.clear();
}

template <typename T> int32_t quadtree_t<T>::execute_each(const treereference_t<quadnode_t<T> >& node, iterationfunc callback)
{
	int32_t result = 1;
	if (node != 0 && !node.empty() && !node->empty() && callback != 0)
	{
		result = callback(node, node->_data);
		// if (result != 0)
		// {
		// 	result = execute_each(node->_left, callback);
		// 	if (result != 0)
		// 	{
		// 		result = execute_each(node->_right, callback);
		// 	}
		// }
	}
	
	return result;
}
template <typename T> int32_t quadtree_t<T>::execute_path(const treereference_t<quadnode_t<T> >& node, iterationfunc callback)
{
	int32_t result = 0;
	if (node != 0 && !node.empty() && !node->empty() && callback != 0)
	{
		result = callback(node, node->_data);
		// if (result != 0)
		// {
		// 	if (result > 0)
		// 	{
		// 		return execute_path(node->_left, callback);
		// 	}
		// 	else
		// 	{
		// 		return execute_path(node->_right, callback);
		// 	}
		// }
	}
	
	return result;
}