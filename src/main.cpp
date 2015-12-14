
#include "../include/tree.h"

#include <stdio.h>

int main(int argc, char** argv)
{
	printf("  starting\n");
	binarynode_t<int>* nodes = 0;
	int32_t num = 0;
	
	printf("  creating tree\n");
	binarytree_t<int> bt0;
	printf("  setting root\n");
	binarytree_t<int>::iterator i = bt0.set_root(2);
	
	printf("  setting node\n");
	i = i.left(8);
	
	printf("  setting node\n");
	i = i.right(4);
	
	printf("  setting node\n");
	i = bt0.root().right(7);
	i.right(3).right(9);
	
	printf("\n");
	nodes = (binarynode_t<int>*)calloc(8, sizeof(binarynode_t<int>));
	num = bt0.nodes(nodes, 8);
	for (int32_t i = 0; i < num; i++)
	{
		binarynode_t<int> node = nodes[i];
		printf("    node (%d, %d) = %d\n", node._ring, node._branch, node._data);
	}
	
	printf("\n");
	
	printf("  removing node\n");
	i.remove();
	
	printf("\n");
	nodes = (binarynode_t<int>*)calloc(64, sizeof(binarynode_t<int>));
	num = bt0.nodes(nodes, 64);
	for (int32_t i = 0; i < num; i++)
	{
		binarynode_t<int> node = nodes[i];
		printf("    node (%d, %d) = %d\n", node._ring, node._branch, node._data);
	}
	
	printf("\n");
	
	printf("  removing node\n");
	bt0.root().remove();
	
	printf("\n");
	nodes = (binarynode_t<int>*)calloc(8, sizeof(binarynode_t<int>));
	num = bt0.nodes(nodes, 8);
	for (int32_t i = 0; i < num; i++)
	{
		binarynode_t<int> node = nodes[i];
		printf("    node (%d, %d) = %d\n", node._ring, node._branch, node._data);
	}
	
	printf("\n");
	
	bt0.clear();
	
	return 0;
}