
#include "../include/tree.h"

#include <stdio.h>

int callback_binary_print(const treereference_t<binarynode_t<int> >& node, const int& item)
{
	printf("    node (%d, %d) = %d\n", node->_ring, node->_branch, item);
	return item / abs(item);
}

int main(int argc, char** argv)
{
	printf("  starting\n");
	
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
	printf("    found 9? %s\n", bt0.search(9).empty() ? "false" : "true");
	
	printf("\n");
	
	printf("  printing tree\n");
	bt0.each(&callback_binary_print);
	
	printf("\n");
	
	printf("  printing tree by path\n");
	bt0.path(&callback_binary_print);
	
	printf("\n");
	
	printf("  removing node (1, 0)\n");
	i.remove();
	
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
	printf("    found 9? %s\n", bt0.search(9).empty() ? "false" : "true");
	
	printf("\n");
	
	bt0.clear();
	
	return 0;
}