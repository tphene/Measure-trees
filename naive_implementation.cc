#include <set>
#include <utility>
#include <limits.h>
#include <stdio.h>
#include "API.h"

#include <stdlib.h>
#include <string.h>

#define NEG_INF -2147483648
#define POS_INF 2147483647
#define STACK_MAX 9999999


#define DEBUG 0

using namespace std;

typedef pair<int, int> interval_t;

typedef struct node_list{
	int left;
	int right;
	struct node_list* next;
}node_list;

typedef struct m_tree_t{
	int key;
	int leftMin;
	int rightMax;
	int height;
	int measure;
	int l,r;
	struct m_tree_t *left;
	struct m_tree_t *right;
	struct node_list *node_list;
}m_tree_t;

m_tree_t *currentblock = NULL;
int    size_left;
m_tree_t *freeList = NULL;
int nodes_taken = 0;
int nodes_returned = 0;
/*
   Utility Operations
*/
int min(int a, int b)
{
	if(a<=b) 
		return a;
	return b;    
}

int max(int a, int b)
{
	if(a>=b) 
		return a;
	return b;
}

void return_node(m_tree_t *node)
{  
	node->right = freeList;
	freeList = node;
	nodes_returned +=1;
}

/*  Stack Operations -- (Referred from the Text Book)*/
/*
   structure for implementing the stack
 */

typedef struct stack {
	m_tree_t *tree;
	struct stack *next;
} stack;


/*
   Creates an Empty Stack
*/
stack *createStack(void)
{
	stack *s = (stack*) malloc(sizeof(stack));
	s->next = NULL;
	return s;
}

/*
   returns 1 if Stack is empty, else 0
 */
int stackEmpty(stack *s)
{
	if(s->next == NULL)
		return 1;
		return 0;
}

/*
   Pushes the object into the Stack
 */
void stackPush( m_tree_t *tree, stack *s)
{
	stack *temp = (stack*) malloc(sizeof(stack));
	temp->tree = tree;
	temp->next = s->next;
	s->next = temp;
}
/*
   returns the top element of the Stack and deletes the item from the Stack
*/
m_tree_t* stackPop(stack *s)
{
	stack *temp; m_tree_t* node;
	temp = s->next;
	s->next = temp->next;
	node = temp->tree;
	free(temp);
	return node;
}

/*
   return the top element from the stack
*/
m_tree_t* stackTop(stack *s) //cs
{
	if( s->next == NULL)
	{
		return NULL;
	}
	return( s->next->tree );
}

void removeStack(stack *s)
{
	stack *temp;
	do
	{
		temp = s->next;
		free(s);
		s = temp;
	}
	while (temp != NULL);
}

/*End of Stack operations*/

m_tree_t* create_node()
{
	m_tree_t *mTree = (m_tree_t*) malloc(sizeof(m_tree_t));
	mTree->leftMin = NEG_INF
;
	mTree->rightMax = POS_INF;
	mTree->left = NULL;
	mTree->height = 0;
	mTree->measure = 0;
	mTree->node_list = NULL;
	return (mTree);
}
/*
   creates an empty measure tree.
*/

m_tree_t * create_m_tree()
{
	m_tree_t *mTree = create_node();
	if(mTree){
		return (mTree);
	}
	return NULL;
}

void destroy_m_tree(m_tree_t *tree)
{
	m_tree_t *node, *temp;
	if(tree->left == NULL)
		return_node(tree);
	else{
		node = tree;
		while(node->right != NULL) 
		{
			if(node->left->right != NULL) //cc
			{
				temp = node->left;
	  			node->left = temp->right;
	  			temp->right = node; 
	  			node = temp;
			}
			else
			{  	
	  			return_node(node->left);
	  			temp = node->right;
	  			return_node(node);
	  			node = temp;
			}
		}
		return_node(node);
	}
}


void calculateLeafMeasure(m_tree_t* tree)
{
	int left, right;
	left = tree->leftMin < tree->l ? tree->l : tree -> leftMin;
	right = tree->rightMax > tree->r ? tree->r : tree->rightMax;
	tree->measure = right - left;
}


/*
Referred from the Text Book
*/
void calculateInternalMeasure(m_tree_t *n)
{
	if(n->right->leftMin < n->l && n->left->rightMax >= n->r)
	    n->measure = n->r - n->l;
	if(n->right->leftMin >= n->l && n->left->rightMax >= n->r)
	    n->measure = n->r - n->key + n->left->measure;
	if(n->right->leftMin < n->l && n->left->rightMax < n->r)
	    n->measure = n->right->measure + n->key - n->l;
	if(n->right->leftMin >= n->l && n->left->rightMax < n->r)
	    n->measure = n->right->measure + n->left->measure;
}

/*
   	Adds the node list to the associated node list
*/
void addNodeInterval(m_tree_t* tree, node_list *list)
{
	node_list *temp =  (node_list*) tree->left;
	node_list *newNode = (node_list*)malloc(sizeof(node_list));
	newNode->left = list->left;
	newNode->right = list->right;
	newNode->next = NULL;
	if(temp == NULL)
	{
		tree->left = (m_tree_t*) newNode;
		return;
	}
	newNode->next = (node_list*)(tree->left);
	tree->left = (m_tree_t*) newNode;
}


int calculateLeftMin(node_list* head)
{
	int minimum = head->left;
	head = head->next;
	while(head!=NULL)
	{
		if(head->left < minimum)
		{
			minimum = head->left;
	  	}
	  	head = head->next;
	}
	return minimum;
}


int calculateRightMax(node_list* head)
{
	int maximum = head->right;
	head = head->next;
	while(head!=NULL)
	{
	    if(head->right > maximum)
	    {
			maximum = head->right;
	    }
		head = head->next;
	}
	return maximum;
}


/*
   Performs Left Rotation on the tree - (Referred from the Text Book)
 */

void leftRotation(m_tree_t *tree)
{
	m_tree_t *temp = tree;
	int key;
	temp = tree->left;
	key = tree->key;
	tree->left = tree->right;
	tree->key = tree->right->key;
	tree->right = tree->left->right;
	tree->left->right = tree->left->left;
	tree->left->left = temp;
	tree->left->key = key;
	tree->left->l = tree->l;
	tree->left->r = tree->key;
	tree->left->leftMin = min(tree->left->left->leftMin,tree->left->right->leftMin);
	tree->left->rightMax = max(tree->left->left->rightMax,tree->left->right->rightMax);
	calculateInternalMeasure(tree->left);
}

/*
   Performs Right Rotation on the tree - (Referred from the Text Book)
 */

void rightRotation(m_tree_t *tree)
{
	m_tree_t *temp = tree;
	int key;
	temp = tree->right;
	key = tree->key;
	tree->right = tree->left;
	tree->key = tree->left->key;
	tree->left = tree->right->left;
	tree->right->left = tree->right->right;
	tree->right->right = temp;
	tree->right->key = key;
	tree->right->l = tree->key;
	tree->right->r = tree->r;

	tree->right->leftMin = min(tree->right->left->leftMin,tree->right->right->leftMin);
	tree->right->rightMax = max(tree->right->left->rightMax,tree->right->right->rightMax);
	calculateInternalMeasure(tree->right);
}

/*
   inserts key value in to the measure tree - Source from the basic search tree implementation - Project1
*/


void insertKey(m_tree_t *tree, int key, node_list *list)
{
	m_tree_t *temp;
	stack *s = createStack();
	stack *rotateStack = createStack();
	int finished = 0;
	if( tree->left != NULL ) //cs
	{
		temp = tree;
	    while( temp->right != NULL )
	    {
		stackPush(temp, s);
		stackPush(temp, rotateStack);
		if(key >= temp->key) //cs
		       temp = temp->right;
		else
		       temp = temp->left;

	    }

	    if(temp->key != key) //cs
	    {
		  	m_tree_t *old_leaf, *new_leaf;
		 	old_leaf = create_node();
		 	old_leaf->left = temp->left;
		 	old_leaf->key = temp->key;
		 	old_leaf->right  = NULL;
		 	old_leaf->height = 0;
		 	old_leaf->leftMin = temp->leftMin;
		 	old_leaf->rightMax = temp->rightMax;

		 	new_leaf = create_node();
		 	addNodeInterval(new_leaf, list);
		 	new_leaf->key = key;
		 	new_leaf->right  = NULL;
		 	new_leaf->height = 0;
		 	new_leaf->leftMin = list->left;
		 	new_leaf->rightMax = list->right;

		 	if( temp->key >= key ) //cs
		 	{   
		 		temp->left  = new_leaf;
		     	temp->right = old_leaf;
		     	new_leaf->l = temp->l;
		     	new_leaf->r = temp->key;
		     	old_leaf->l = temp->key;
		     	old_leaf->r = temp->r;
		 	}
		 	else
		 	{   
		     temp->left  = old_leaf;
		     temp->right = new_leaf;
		     temp->key = key;
		     old_leaf->l = temp->l;
		     old_leaf->r = key;
		     new_leaf->l = key;
		     new_leaf->r = temp->r;
		 	}
		 	calculateLeafMeasure(old_leaf);
		 	calculateLeafMeasure(new_leaf);

		 	temp->leftMin = min(temp->left->leftMin,temp->right->leftMin);
		 	temp->rightMax = max(temp->left->rightMax,temp->right->rightMax);
		 	stackPush(temp, s);

	    }
	    else
	    {

	    	addNodeInterval(temp, list);
		  temp->leftMin = min(temp->leftMin, list->left);
		  temp->rightMax = max(temp->rightMax, list->right);
		  calculateLeafMeasure(temp);

		 	
	     }
	      
	}
		else
		{
	     node_list *newList =(node_list*)malloc(sizeof(node_list));
	      newList->left = list->left;
	      newList->right = list->right;
	      newList->next = NULL;
	      
	      tree->left = (m_tree_t *) newList;
	      tree->node_list = newList;
	      tree->key  = key;
	      tree->right  = NULL;
	      tree->leftMin = list->left;
	      tree->rightMax = list->right;
	      tree->l = NEG_INF
	     ;
	      tree->r = POS_INF;
	      tree->node_list = newList;
	      calculateLeafMeasure(tree);
		}
	   // update measures.

	while(!stackEmpty(s))
	{
	       temp = stackTop(s);
	       stackPop(s);
	       calculateInternalMeasure(temp);
	       temp->leftMin = min(temp->left->leftMin,temp->right->leftMin);
	       temp->rightMax = max(temp->left->rightMax,temp->right->rightMax);
	}

	   //Rotation Part	- (Referred from the Text Book)

    while(!stackEmpty(rotateStack) && !finished)
	{
	  	temp = stackTop(rotateStack);
	  	stackPop(rotateStack);
	  	int temp_height, old_height;
	  	old_height = temp->height;
		if( temp->left->height - temp->right->height == 2 )
		{
				if( temp->left->left->height - temp->right->height != 1 ) //cs
				{
					leftRotation( temp->left );
					rightRotation( temp );
					temp_height = temp->left->left->height;
					temp->left->height = temp_height + 1;
					temp->right->height = temp_height + 1;
					temp->height = temp_height + 2;
					
				}
				else
				{
					rightRotation( temp );
					temp->right->height = temp->right->left->height + 1;
					temp->height = temp->right->height + 1;
				}
		}
		else if( temp->left->height - temp->right->height == -2 )
		{
				if( temp->right->right->height - temp->left->height != 1 ) //cs
				{
					rightRotation( temp->right );
					leftRotation( temp );
					temp_height =
						temp->right->right->height;
					temp->left->height =
						temp_height + 1;
					temp->right->height =
						temp_height + 1;
					temp->height = temp_height + 2;

				}
				else 
				{
					leftRotation( temp );
					temp->left->height =
					temp->left->right->height + 1;
					temp->height =
					temp->left->height + 1;
				}
		}
			
		else
		{
			if( temp->left->height <= temp->right->height ) //cs
				temp->height = temp->right->height + 1;
			else
				temp->height = temp->left->height + 1;
		}
		if( temp->height == old_height )
				finished = 1;


	}

	removeStack(s);
	removeStack(rotateStack);
}

/*
   inserts the interval [a,b[.
*/
void insert_interval(m_tree_t * tree, int a, int b)
{
	node_list *list = (node_list*) malloc(sizeof(node_list));
	list->left = a;
	list->right = b;
	list->next = NULL;
	if(a>b)
		return;
	insertKey(tree, a, list);
	insertKey(tree, b, list);
	free(list);
}


/*
   Deleting interval from the corresponding node
*/
void deleteIntervalNode(m_tree_t* temp_node, node_list* interval)
{
	node_list* temp = (node_list*)temp_node->left;
	node_list* next = temp->next;
	if(temp!= NULL)
	{
		if(temp->left == interval->left && temp->right == interval->right) //two step change
		{
			temp_node->left = (m_tree_t*) next;
			free(temp);
			return;
		}
		
	}
	while(next!=NULL)
	{
		if(next->left == interval->left && next->right == interval->right)
		{
			temp->next = next->next;
			free(next);
			return;
		}
		temp = temp->next;
		next = next->next;
	}
}


/*
   deletes key value in to the measure tree - Source from the basic search tree implementation - Project1
*/
void deleteKey(m_tree_t *tree, int key, node_list *list)
{
 	  m_tree_t *temp, *upperNode, *sibling;
      int finished = 0;
	  stack *s = createStack();
	  stack *rotateStack = createStack();
	  	if(tree->right == NULL) //cs and cc
	  	{
	  		if(tree->key == key)
	      	{
		 	free(tree->left);
		 	tree->left = NULL;
		 	tree->left = NULL;
		 	tree->key  = 0;
		 	tree->right  = NULL;
		 	tree->leftMin = 0;
		 	tree->rightMax = 0;
		 	tree->l = NEG_INF
		 ;
		 	tree->r = POS_INF;
		 	tree->measure = 0;

	      	}
	      	return;
	  	}
	  	else if( tree->left == NULL ) //cs and cc
	  	{	
	    	return;
	   	}
	   	else
	   	{  
	   		temp = tree;
	      	while(temp->right != NULL)
	      	{   
	      		upperNode = temp;
		  		stackPush(temp, s);
		  		stackPush(temp, rotateStack);
		  		if( key >= temp->key ) //cs
		  		{  
		  			temp   = upperNode->right;
		    		sibling = upperNode->left;
		  		}
		  		else
		  		{  
		     		temp   = upperNode->left;
		     		sibling = upperNode->right;
		  		}
	      	}
	      	if(temp->key != key)
		 	return;

	      	deleteIntervalNode(temp, list);

	      	if(temp->left != NULL) //cs
	      	{ 

	      		temp->leftMin = calculateLeftMin((node_list*)temp->left);
				temp->rightMax = calculateRightMax((node_list*)temp->left);
				calculateLeafMeasure(temp); 

	      	}
	      	else
	      	{
	      		upperNode->key   = sibling->key;
		 		upperNode->left  = sibling->left;
		 		upperNode->right = sibling->right;
		 		upperNode->leftMin = sibling->leftMin;
		 		upperNode->rightMax = sibling->rightMax;
		 		upperNode->measure = sibling->measure;
		 		upperNode->height = sibling->height;
		 		if(upperNode->right != NULL)
		 		{
		      		upperNode->right->r = upperNode->r;
		      		upperNode->left->l = upperNode->l;
		 		}

	        	calculateLeafMeasure(upperNode);

	        	stackPop(rotateStack);
		 		stackPop(s);
		 	
			
	      	}
	   }

	   while(!stackEmpty(s))
	   {
	       temp = stackTop(s);
	       stackPop(s);
	       calculateInternalMeasure(temp);
	       temp->leftMin = min(temp->right->leftMin,temp->left->leftMin); //cs
	       temp->rightMax = max(temp->right->rightMax,temp->left->rightMax); //cs
	   }
	   //Rotation Part	- (Referred from the Text Book)

   	   while(!stackEmpty(rotateStack) && !finished)
   	   {
		  temp = stackTop(rotateStack);
		  stackPop(rotateStack);
		  int temp_height, old_height;
		  old_height= temp->height;

	  		if(temp->left->height - temp->right->height == 2 )
	    	{
	      		if( temp->left->left->height - temp->right->height != 1 ) //cs
				{
		  			leftRotation(temp->left);
		  			rightRotation( temp );
		  			temp_height = temp->left->left->height;
		  			temp->left->height = temp_height + 1;
		  			temp->right->height = temp_height + 1;
		  			temp->height = temp_height + 2;
				}	
	      		else
				{
		  			rightRotation( temp );
		  			temp->right->height = temp->right->left->height + 1;
		  			temp->height = temp->right->height + 1;
				}
	    	}
	  		else if( temp->left->height - temp->right->height == -2 )
	    	{
	      		if( temp->right->right->height - temp->left->height != 1 ) //cs
				{
		  		rightRotation( temp->right );
		  		leftRotation( temp );
		  		temp_height = temp->right->right->height;
		  		temp->left->height = temp_height + 1;
		  		temp->right->height = temp_height + 1;
		  		temp->height = temp_height + 2;

				}
	      		else
				{
		  		leftRotation( temp );
		  		temp->left->height = temp->left->right->height + 1;
		  		temp->height = temp->left->height + 1;
				}
	    	}
	  		else if( temp->left != NULL && temp->right != NULL)
	    	{
	      		if( temp->left->height <= temp->right->height ) //cs
				{	
		  			temp->height = temp->right->height + 1;
				}
	      		else
				{
		  			temp->height = temp->left->height + 1;
				}
	    	}
	  		if(temp->height == old_height)
	    	finished = 1;
	   }
	   removeStack(s);
	   removeStack(rotateStack);
}
// deletes the interval [a,b[, if it exists.
void delete_interval(m_tree_t * tree, int a, int b)
{
	node_list *list = (node_list*) malloc(sizeof(node_list));
	list->left = a;
	list->right = b;
	list->next = NULL;
	deleteKey(tree, a, list);
	deleteKey(tree, b, list);
	free(list);
}
//returns the length of the union of all intervals in the current set
int query_length(m_tree_t * tree)
{
	if(tree == NULL)
		return -1;
	return tree->measure;
}




