#pragma once

template <typename T> inline bool quadnode_t<T>::empty() const
{
	return this->_data == 0 || this->_tree == 0 || this->_ring < 0 || this->_branch < 0;
}

template <typename T> inline size_t quadnode_t<T>::index() const
{
	return tree_index(this->_ring, this->_branch, 4);
}
