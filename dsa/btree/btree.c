#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BT_MAX_CHILDREN(degree) (2 * degree)
#define BT_MAX_KEYS(degree) (2 * degree - 1)

typedef struct BTNode BTNode;
typedef struct BTNode {
	size_t degree;              // Minimum degree (t)
	int* keys;                  // Array to store keys
	size_t key_count;           // Current number of keys

	BTNode** children;			// Array to store children
	size_t children_count;      // Current number of children

	int is_leaf;                // 1 if the node is a leaf, otherwise 0
} BTNode;

typedef struct {
	size_t degree;
	BTNode* root;
} BTree;




BTNode* createBTNode(size_t degree)
{
	BTNode* node = (BTNode*)malloc(sizeof(BTNode));	
	if(!node) return NULL;

	node->keys = (int*)malloc(BT_MAX_KEYS(degree) * sizeof(int));
	node->key_count = 0;
	if(!node->keys){
		free(node);
		return NULL;
	}

	node->children = (BTNode**)malloc(BT_MAX_CHILDREN(degree) * sizeof(BTNode*));
	node->children_count = 0;
	if(!node->children){
		free(node->keys);
		free(node);
		return NULL;
	}

	node->is_leaf = 1; // Initially, all nodes are leaf nodes
	node->degree = degree;
	return node;
}

// Function to create the B-tree
BTree* createBTree(size_t degree) {
    BTree* tree = (BTree*)malloc(sizeof(BTree));
    if (!tree) return NULL;

    tree->degree = degree;
    tree->root = createBTNode(degree);
    if (!tree->root) {
        free(tree);
        return NULL;
    }

    return tree;
}


// Split a child node that is full
void splitChild(BTNode* parent, int childIndex)
{
	BTNode* newChild = createBTNode(parent->degree);
	BTNode* child = parent->children[childIndex];
} 

// Insert a key into a non-full node
void insertNonFull(BTNode* node, int key)
{
	int index = node->key_count - 1; // Last key index	
	
	if(node->is_leaf)
	{
		// Find correct place for new key
		while(index >= 0 && node->keys[index] > key)
		{
			// Shift key to the right
			node->keys[index + 1] = node->keys[index];
			index--;
		}

		// Insert new key into node
		node->keys[index + 1] = key;
		node->key_count++;
	} 
	else 
	{
		// Find right child index to goto
		while(index >= 0 && node->keys[index] > key)
		{
			index--;
		}
		index++; // Please add better comment to why we need this

		// Check if we need to split child node
		if(node->children[index]->key_count >= BT_MAX_CHILDREN(node->children[index]->degree))
		{

		}

		// Recusivly untill we find valid leaf node
		insertNonFull(node->children[index], key);
	}
}


void btreeInsert(BTree* tree, int key)
{
	BTNode* root = tree->root;
	if(root->key_count >= BT_MAX_KEYS(root->degree)){
		printf("LOL1\n");
	} else {
		insertNonFull(root, key);
	}	
}



int main()
{
    BTree* tree = createBTree(3);
    btreeInsert(tree, 4);
    btreeInsert(tree, 2);
    btreeInsert(tree, 1);
    btreeInsert(tree, 3);

    printBTree(tree); // Call to print the whole B-Tree

    return 0;
}
