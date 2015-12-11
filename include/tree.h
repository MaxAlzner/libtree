#pragma once

template <typename T> class binarytree_t
{
public:

	typedef struct node_t node_t;
	typedef struct iterator iterator;

	inline binarytree_t() {}
	inline ~binarytree_t() {}

	struct node_t
	{

		inline node_t() :
			_left(0), _right(0), _tree(0), _data(0) {}
		inline node_t(const binarytree_t* tree, const T* data) :
			_left(0), _right(0), _tree(tree), _data(data) {}
		inline ~node_t() {}

		inline iterator set_left(const T& item);
		inline iterator set_right(const T& item);

		inline iterator left() const { return iterator(*(this->_left)); }
		inline iterator right() const { return iterator(*(this->_right)); }

		node_t* _left;
		node_t* _right;
		binarytree_t* _tree;
		const T* _data;

	};

	struct iterator
	{

		inline iterator() {}
		inline iterator(const node_t& node) : node(node) {}
		inline ~iterator() {}

		inline iterator& operator++()
		{
			return *this;
		}
		inline iterator& operator--()
		{
			return *this;
		}
		inline T& operator*() const { return *(this->node.data); }

		node_t node;

	};

	inline iterator set_root(const T& item);

	inline iterator root() const { return iterator(this->_root); }

protected:

	std::vector<node_t> _registry;
	node_t* _root;

};