
#include "../include/tree.h"

#include <stdio.h>

int main(int argc, char** argv)
{
	printf("  starting\n");
	
	printf("  creating tree\n");
	binarytree_t<int> bt0;
	printf("  setting root\n");
	binarytree_t<int>::iterator i = bt0.set_root(2);
	
	printf("  setting node\n");
	i = i.left(8);
	
	printf("  setting node\n");
	i = i.right(4);
	
	printf("  setting node\n");
	bt0.root().right(7);
	
	printf("  printing tree\n");
	binarynode_t<int>* nodes = (binarynode_t<int>*)calloc(8, sizeof(binarynode_t<int>));
	int32_t num = bt0.nodes(nodes, 8);
	for (int32_t i = 0; i < num; i++)
	{
		binarynode_t<int> node = nodes[i];
		printf("    node (%d, %d) = %d\n", node._ring, node._branch, node._data);
	}
	
	return 0;
}