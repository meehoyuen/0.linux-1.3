/*
 * this code is derived from the avl functions in mmap.c
 */
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/malloc.h>
#include <linux/skbuff.h>

#include <net/br.h>
#define _DEBUG_AVL

/*
 * Use an AVL (Adelson-Velskii and Landis) tree to speed up this search
 * from O(n) to O(log n), where n is the number of ULAs.
 * Written by Bruno Haible <haible@ma2s2.mathematik.uni-karlsruhe.de>.
 * Taken from mmap.c, extensively modified by John Hayes 
 * <hayes@netplumbing.com>
 */

struct fdb fdb_head;
struct fdb *fhp = &fdb_head;
struct fdb **fhpp = &fhp;
static int fdb_inited = 0;

int addr_cmp(unsigned char *a1, unsigned char *a2);
static void printk_avl (struct fdb * tree);

/*
 * fdb_head is the AVL tree corresponding to fdb
 * or, more exactly, its root.
 * A fdb has the following fields:
 *   fdb_avl_left     left son of a tree node
 *   fdb_avl_right    right son of a tree node
 *   fdb_avl_height   1+max(heightof(left),heightof(right))
 * The empty tree is represented as NULL.
 */
 
#ifndef avl_br_empty
#define avl_br_empty	(struct fdb *) NULL
#endif

/* Since the trees are balanced, their height will never be large. */
#define avl_maxheight	127
#define heightof(tree)	((tree) == avl_br_empty ? 0 : (tree)->fdb_avl_height)
/*
 * Consistency and balancing rules:
 * 1. tree->fdb_avl_height == 1+max(heightof(tree->fdb_avl_left),heightof(tree->fdb_avl_right))
 * 2. abs( heightof(tree->fdb_avl_left) - heightof(tree->fdb_avl_right) ) <= 1
 * 3. foreach node in tree->fdb_avl_left: node->fdb_avl_key <= tree->fdb_avl_key,
 *    foreach node in tree->fdb_avl_right: node->fdb_avl_key >= tree->fdb_avl_key.
 */

int
fdb_init(void)
{
	fdb_head.fdb_avl_height = 0;
	fdb_head.fdb_avl_left = (struct fdb *)0;
	fdb_head.fdb_avl_right = (struct fdb *)0;
	fdb_inited = 1;
	return(0);
}

struct fdb *
br_avl_find_addr(unsigned char addr[6])
{
	struct fdb * result = NULL;
	struct fdb * tree;

	if (!fdb_inited)
		fdb_init();
#if (DEBUG_AVL)
	printk("searching for ula %02x:%02x:%02x:%02x:%02x:%02x\n",
		addr[0],
		addr[1],
		addr[2],
		addr[3],
		addr[4],
		addr[5]);
#endif /* DEBUG_AVL */
	for (tree = &fdb_head ; ; ) {
		if (tree == avl_br_empty) {
#if (DEBUG_AVL)
			printk("search failed, returning node 0x%x\n", (unsigned int)result);
#endif /* DEBUG_AVL */
			return result;
		}

#if (DEBUG_AVL)
		printk("node 0x%x: checking ula %02x:%02x:%02x:%02x:%02x:%02x\n",
			(unsigned int)tree,
			tree->ula[0],
			tree->ula[1],
			tree->ula[2],
			tree->ula[3],
			tree->ula[4],
			tree->ula[5]);
#endif /* DEBUG_AVL */
		if (addr_cmp(addr, tree->ula) == 0) {
#if (DEBUG_AVL)
			printk("found node 0x%x\n", (unsigned int)tree);
#endif /* DEBUG_AVL */
			return tree;
		}
		if (addr_cmp(addr, tree->ula) < 0) {
			tree = tree->fdb_avl_left;
		} else {
			tree = tree->fdb_avl_right;
		}
	}
}

/*
 * Rebalance a tree.
 * After inserting or deleting a node of a tree we have a sequence of subtrees
 * nodes[0]..nodes[k-1] such that
 * nodes[0] is the root and nodes[i+1] = nodes[i]->{fdb_avl_left|fdb_avl_right}.
 */
static void 
br_avl_rebalance (struct fdb *** nodeplaces_ptr, int count)
{
	if (!fdb_inited)
		fdb_init();
	for ( ; count > 0 ; count--) {
		struct fdb ** nodeplace = *--nodeplaces_ptr;
		struct fdb * node = *nodeplace;
		struct fdb * nodeleft = node->fdb_avl_left;
		struct fdb * noderight = node->fdb_avl_right;
		int heightleft = heightof(nodeleft);
		int heightright = heightof(noderight);
		if (heightright + 1 < heightleft) {
			/*                                                      */
			/*                            *                         */
			/*                          /   \                       */
			/*                       n+2      n                     */
			/*                                                      */
			struct fdb * nodeleftleft = nodeleft->fdb_avl_left;
			struct fdb * nodeleftright = nodeleft->fdb_avl_right;
			int heightleftright = heightof(nodeleftright);
			if (heightof(nodeleftleft) >= heightleftright) {
				/*                                                        */
				/*                *                    n+2|n+3            */
				/*              /   \                  /    \             */
				/*           n+2      n      -->      /   n+1|n+2         */
				/*           / \                      |    /    \         */
				/*         n+1 n|n+1                 n+1  n|n+1  n        */
				/*                                                        */
				node->fdb_avl_left = nodeleftright; 
				nodeleft->fdb_avl_right = node;
				nodeleft->fdb_avl_height = 1 + (node->fdb_avl_height = 1 + heightleftright);
				*nodeplace = nodeleft;
			} else {
				/*                                                        */
				/*                *                     n+2               */
				/*              /   \                 /     \             */
				/*           n+2      n      -->    n+1     n+1           */
				/*           / \                    / \     / \           */
				/*          n  n+1                 n   L   R   n          */
				/*             / \                                        */
				/*            L   R                                       */
				/*                                                        */
				nodeleft->fdb_avl_right = nodeleftright->fdb_avl_left;
				node->fdb_avl_left = nodeleftright->fdb_avl_right;
				nodeleftright->fdb_avl_left = nodeleft;
				nodeleftright->fdb_avl_right = node;
				nodeleft->fdb_avl_height = node->fdb_avl_height = heightleftright;
				nodeleftright->fdb_avl_height = heightleft;
				*nodeplace = nodeleftright;
			}
		} else if (heightleft + 1 < heightright) {
			/* similar to the above, just interchange 'left' <--> 'right' */
			struct fdb * noderightright = noderight->fdb_avl_right;
			struct fdb * noderightleft = noderight->fdb_avl_left;
			int heightrightleft = heightof(noderightleft);
			if (heightof(noderightright) >= heightrightleft) {
				node->fdb_avl_right = noderightleft; 
				noderight->fdb_avl_left = node;
				noderight->fdb_avl_height = 1 + (node->fdb_avl_height = 1 + heightrightleft);
				*nodeplace = noderight;
			} else {
				noderight->fdb_avl_left = noderightleft->fdb_avl_right;
				node->fdb_avl_right = noderightleft->fdb_avl_left;
				noderightleft->fdb_avl_right = noderight;
				noderightleft->fdb_avl_left = node;
				noderight->fdb_avl_height = node->fdb_avl_height = heightrightleft;
				noderightleft->fdb_avl_height = heightright;
				*nodeplace = noderightleft;
			}
		} else {
			int height = (heightleft<heightright ? heightright : heightleft) + 1;
			if (height == node->fdb_avl_height)
				break;
			node->fdb_avl_height = height;
		}
	}
#ifdef DEBUG_AVL
	printk_avl(&fdb_head);
#endif /* DEBUG_AVL */
}

/* Insert a node into a tree. */
int 
br_avl_insert (struct fdb * new_node)
{
	struct fdb ** nodeplace = fhpp;
	struct fdb ** stack[avl_maxheight];
	int stack_count = 0;
	struct fdb *** stack_ptr = &stack[0]; /* = &stack[stackcount] */
	if (!fdb_inited)
		fdb_init();
	for (;;) {
		struct fdb *node;
		
		node = *nodeplace;
		if (node == avl_br_empty)
			break;
		*stack_ptr++ = nodeplace; stack_count++;
		if (addr_cmp(new_node->ula, node->ula) == 0) { /* update */
			node->flags = new_node->flags;
			node->timer = new_node->timer;	
			return(0);
		}
		if (addr_cmp(new_node->ula, node->ula) < 0) {
			nodeplace = &node->fdb_avl_left;
		} else {
			nodeplace = &node->fdb_avl_right;
		}
	}
#if (DEBUG_AVL)
	printk("node 0x%x: adding ula %02x:%02x:%02x:%02x:%02x:%02x\n",
		(unsigned int)new_node,
		new_node->ula[0],
		new_node->ula[1],
		new_node->ula[2],
		new_node->ula[3],
		new_node->ula[4],
		new_node->ula[5]);
#endif /* (DEBUG_AVL) */
	new_node->fdb_avl_left = avl_br_empty;
	new_node->fdb_avl_right = avl_br_empty;
	new_node->fdb_avl_height = 1;
	*nodeplace = new_node;
#if (0)	
	br_avl_rebalance(stack_ptr,stack_count);
#endif /* (0) */
#ifdef DEBUG_AVL
	printk_avl(&fdb_head);
#endif /* DEBUG_AVL */
	return(1);
}

/* Removes a node out of a tree. */
int
br_avl_remove (struct fdb * node_to_delete)
{
	struct fdb ** nodeplace = fhpp;
	struct fdb ** stack[avl_maxheight];
	int stack_count = 0;
	struct fdb *** stack_ptr = &stack[0]; /* = &stack[stackcount] */
	struct fdb ** nodeplace_to_delete;
	if (!fdb_inited)
		fdb_init();
	for (;;) {
		struct fdb * node = *nodeplace;
		if (node == avl_br_empty) {
			/* what? node_to_delete not found in tree? */
			printk("avl_remove: node to delete not found in tree\n");
			return(-1);
		}
		*stack_ptr++ = nodeplace; stack_count++;
		if (addr_cmp(node_to_delete->ula, node->ula) == 0)
				break;
		if (addr_cmp(node_to_delete->ula, node->ula) < 0)
			nodeplace = &node->fdb_avl_left;
		else
			nodeplace = &node->fdb_avl_right;
	}
	nodeplace_to_delete = nodeplace;
	/* Have to remove node_to_delete = *nodeplace_to_delete. */
	if (node_to_delete->fdb_avl_left == avl_br_empty) {
		*nodeplace_to_delete = node_to_delete->fdb_avl_right;
		stack_ptr--; stack_count--;
	} else {
		struct fdb *** stack_ptr_to_delete = stack_ptr;
		struct fdb ** nodeplace = &node_to_delete->fdb_avl_left;
		struct fdb * node;
		for (;;) {
			node = *nodeplace;
			if (node->fdb_avl_right == avl_br_empty)
				break;
			*stack_ptr++ = nodeplace; stack_count++;
			nodeplace = &node->fdb_avl_right;
		}
		*nodeplace = node->fdb_avl_left;
		/* node replaces node_to_delete */
		node->fdb_avl_left = node_to_delete->fdb_avl_left;
		node->fdb_avl_right = node_to_delete->fdb_avl_right;
		node->fdb_avl_height = node_to_delete->fdb_avl_height;
		*nodeplace_to_delete = node; /* replace node_to_delete */
		*stack_ptr_to_delete = &node->fdb_avl_left; /* replace &node_to_delete->fdb_avl_left */
	}
	br_avl_rebalance(stack_ptr,stack_count);
	return(0);
}

#ifdef DEBUG_AVL

/* print a tree */
static void printk_avl (struct fdb * tree)
{
	if (tree != avl_br_empty) {
		printk("(");
		printk("%02x:%02x:%02x:%02x:%02x:%02x",
			tree->ula[0],
			tree->ula[1],
			tree->ula[2],
			tree->ula[3],
			tree->ula[4],
			tree->ula[5]);
		if (tree->fdb_avl_left != avl_br_empty) {
			printk_avl(tree->fdb_avl_left);
			printk("<");
		}
		if (tree->fdb_avl_right != avl_br_empty) {
			printk(">");
			printk_avl(tree->fdb_avl_right);
		}
		printk(")\n");
	}
}

#if (0)
static char *avl_check_point = "somewhere";

/* check a tree's consistency and balancing */
static void avl_checkheights (struct fdb * tree)
{
	int h, hl, hr;

	if (tree == avl_br_empty)
		return;
	avl_checkheights(tree->fdb_avl_left);
	avl_checkheights(tree->fdb_avl_right);
	h = tree->fdb_avl_height;
	hl = heightof(tree->fdb_avl_left);
	hr = heightof(tree->fdb_avl_right);
	if ((h == hl+1) && (hr <= hl) && (hl <= hr+1))
		return;
	if ((h == hr+1) && (hl <= hr) && (hr <= hl+1))
		return;
	printk("%s: avl_checkheights: heights inconsistent\n",avl_check_point);
}

/* check that all values stored in a tree are < key */
static void avl_checkleft (struct fdb * tree, fdb_avl_key_t key)
{
	if (tree == avl_br_empty)
		return;
	avl_checkleft(tree->fdb_avl_left,key);
	avl_checkleft(tree->fdb_avl_right,key);
	if (tree->fdb_avl_key < key)
		return;
	printk("%s: avl_checkleft: left key %lu >= top key %lu\n",avl_check_point,tree->fdb_avl_key,key);
}

/* check that all values stored in a tree are > key */
static void avl_checkright (struct fdb * tree, fdb_avl_key_t key)
{
	if (tree == avl_br_empty)
		return;
	avl_checkright(tree->fdb_avl_left,key);
	avl_checkright(tree->fdb_avl_right,key);
	if (tree->fdb_avl_key > key)
		return;
	printk("%s: avl_checkright: right key %lu <= top key %lu\n",avl_check_point,tree->fdb_avl_key,key);
}

/* check that all values are properly increasing */
static void avl_checkorder (struct fdb * tree)
{
	if (tree == avl_br_empty)
		return;
	avl_checkorder(tree->fdb_avl_left);
	avl_checkorder(tree->fdb_avl_right);
	avl_checkleft(tree->fdb_avl_left,tree->fdb_avl_key);
	avl_checkright(tree->fdb_avl_right,tree->fdb_avl_key);
}

#endif /* (0) */
#endif /* DEBUG_AVL */

int
addr_cmp(unsigned char a1[], unsigned char a2[])
{
	int i;

	for (i=0; i<6; i++) {
		if (a1[i] > a2[i]) return(1);
		if (a1[i] < a2[i]) return(-1);
	}
	return(0);
}

