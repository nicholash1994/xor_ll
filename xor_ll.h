/*

This library contains code to create and manipulate doubly-linked lists.
Unlike normal doubly-linked lists, however, the "next" and "prev" pointers
of each list node are XOR'd together, resulting in about a 50% reduction
in space overhead. Of course, this also incurs some runtime overhead, 
because the pointers to the next (or previous) node have to be extracted 
from the XOR field every time one moves up or down the list.

This library also utilizes a freelist in order to avoid having to call free
and malloc every time one wants to add or remove a node in the list.

*/

#ifndef XOR_LL_H
#define XOR_LL_H

// the struct for the nodes of the linked list
typedef struct xor_node {
	void* data;
	struct xor_node* xor;
} xor_node_t; 

// the struct for the linked list itself
typedef struct xor_ll {
	unsigned int size, pos, fsize;
	xor_node_t* next;
	xor_node_t* prev;
	xor_node_t* curr;
	xor_node_t* freelist;
	void* (*create)();
	void (*destroy)(void*);
} xor_ll_t;

xor_ll_t create_xor_ll(void* (*create)(), void (*destroy)(void*));
void move_to_pos_xor_ll(xor_ll_t* llist, unsigned int pos);
void next_xor_ll(xor_ll_t* llist);
void prev_xor_ll(xor_ll_t* llist);
void add_node_b(xor_ll_t* llist);
void add_node_f(xor_ll_t* llist);
xor_node_t* get_node(xor_ll_t*);
void remove_current_node(xor_ll_t*);
void clear_freelist(xor_ll_t*);
void print_xor_ll_info(xor_ll_t*);


#endif
