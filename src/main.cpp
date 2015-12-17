
#include "../include/tree.h"

#include <stdio.h>

#include <string>

int callback_binary_print(const treereference_t<binarynode_t<int> >& node, const int& item)
{
	printf("    node (%d, %d) = %d\n", node->_ring, node->_branch, item);
	return item / abs(item);
}

int callback_quad_print(const treereference_t<quadnode_t<std::string> >& node, const std::string& item)
{
	printf("    node (%d, %d) = %s\n", node->_ring, node->_branch, item.c_str());
	return 1;
}

void binary_test()
{
	printf("  starting binary tree\n");
	
	printf("  creating tree\n");
	binarytree_t<int> bt0;
	
	printf("  setting root\n");
	binarytree_t<int>::iterator i = bt0.set_root(2);
	
	printf("  setting node\n");
	i = i.left(8);
	i.left(2).left(5);
	i.right(4);
	
	printf("  setting node\n");
	i = bt0.root().right(7);
	i.right(3).right(9);
	
	printf("\n");
	
	printf("    found 8? %s\n", bt0.search(8).empty() ? "false" : "true");
	printf("    found -10? %s\n", bt0.search(-10).empty() ? "false" : "true");
	printf("    found 2? %s\n", bt0.search(2).empty() ? "false" : "true");
	printf("    found 3? %s\n", bt0.search(3).empty() ? "false" : "true");
	
	printf("\n");
	
	printf("  printing tree\n");
	bt0.each(&callback_binary_print);
	
	printf("\n");
	
	printf("  printing tree by path\n");
	bt0.path(&callback_binary_print);
	
	printf("\n");
	
	printf("  removing node (0, 1)\n");
	bt0.root().left().remove();
	
	printf("\n");
	
	printf("    found 8? %s\n", bt0.search(8).empty() ? "false" : "true");
	printf("    found -10? %s\n", bt0.search(-10).empty() ? "false" : "true");
	printf("    found 2? %s\n", bt0.search(2).empty() ? "false" : "true");
	printf("    found 3? %s\n", bt0.search(3).empty() ? "false" : "true");
	
	printf("\n");
	
	printf("  printing tree\n");
	bt0.each(&callback_binary_print);
	
	printf("\n");
	
	printf("  removing node (0, 0)\n");
	bt0.root().remove();
	
	printf("\n");
	
	printf("  printing tree\n");
	bt0.each(&callback_binary_print);
	
	printf("\n");
	
	printf("    found 8? %s\n", bt0.search(8).empty() ? "false" : "true");
	printf("    found -10? %s\n", bt0.search(-10).empty() ? "false" : "true");
	printf("    found 2? %s\n", bt0.search(2).empty() ? "false" : "true");
	printf("    found 3? %s\n", bt0.search(3).empty() ? "false" : "true");
	
	printf("\n");
	
	bt0.clear();
}

void quad_test()
{
	printf("  starting binary tree\n");
	
	printf("  creating tree\n");
	quadtree_t<std::string> bt0;
	
	printf("  setting root\n");
	quadtree_t<std::string>::iterator i = bt0.set_root("I am a root");
	
	printf("  printing tree\n");
	bt0.each(&callback_quad_print);
}

int main(int argc, char** argv)
{
	for (int i = 0; i < argc; i++)
	{
		std::string option = argv[i];
		if (option.size() > 1 && option[0] == '-')
		{
			option = option.substr(1);
			if (option == "binary")
			{
				binary_test();
			}
			else if (option == "quad")
			{
				quad_test();
			}
		}
	}
	
	return 0;
}