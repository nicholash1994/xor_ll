#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "xor_ll.h"

#define XOR2(a, b) ((uintptr_t)a ^ (uintptr_t)b)
#define XOR(a, b, c) ((uintptr_t) a ^ (uintptr_t) b ^ (uintptr_t) c)


// function to create an empty doubly linked list
xor_ll_t create_xor_ll(void* (*create)(), void (*destroy)(void*)) {
	xor_ll_t llist;
	
	// in the comments, these three pointers will collectively be
	// referred to as the cursor, since they merely point to nodes
	// in the underlying linked list, and help us navigate the 
	// linked list.
	llist.next = NULL;
	llist.prev = NULL;
	llist.curr = NULL;

	// how many elements are in the list
	llist.size = 0;

	// self-explanatory
	llist.pos = -1;

	// freelist size
	llist.fsize = 0;
	
	// constructor and destructor functions
	llist.create = create;
	llist.destroy = destroy;
	
	llist.freelist = NULL;

	return llist;
}

// move up one node in the linked list
void next_xor_ll(xor_ll_t* llist) {
	llist->prev = llist->curr;
	llist->curr = llist->next;
	llist->next = (xor_node_t*)XOR2(llist->curr->xor, llist->prev);
	llist->pos++;
}

// move down one node in the linked list
void prev_xor_ll(xor_ll_t* llist) {
	llist->next = llist->curr;
	llist->curr = llist->prev;
	llist->prev = (xor_node_t*)XOR2(llist->curr->xor, llist->next);
	llist->pos--;
}

// move the position specified by pos
// if pos is out of bounds, the function simply returns
void move_to_pos_xor_ll(xor_ll_t* llist, unsigned int pos) {

	if (pos < 0 || pos >= llist->size)
		return;
	
	if (llist->pos > pos)
		while (llist->pos > pos)
			prev_xor_ll(llist);
	else if (llist->pos < pos)
		while (llist->pos < pos)
			next_xor_ll(llist);
}

// add node to the list
void add_node_b(xor_ll_t* llist) {
	xor_node_t* new_node;
	
	new_node = get_node(llist);

	if (llist->curr == NULL) {
		llist->curr = new_node;
		llist->curr->xor = NULL;
	}
	else if (llist->prev == NULL) {
		llist->prev = new_node;
		llist->prev->xor = llist->curr;
		llist->curr->xor = (xor_node_t*)XOR2(llist->prev, llist->next);
	}
	else {
		llist->prev->xor = 
			(xor_node_t*)XOR(llist->prev->xor, llist->curr, new_node);
		llist->curr->xor =
			(xor_node_t*)XOR(llist->curr->xor, llist->prev, new_node);
		new_node->xor = (xor_node_t*)XOR2(llist->prev, llist->curr);
		llist->prev = new_node;
	}

	llist->pos++;
	llist->size++;
}

// all functions which add nodes to the list ultimately call one 
// of these two functions
void add_node_f(xor_ll_t* llist) {
	xor_node_t* new_node;

	new_node = get_node(llist);

	if (llist->curr == NULL) {
		llist->curr = new_node;
		llist->curr->xor = NULL;
	}
	else if (llist->next == NULL) {
		llist->next = new_node;
		llist->next->xor = (xor_node_t*)XOR2(llist->curr, NULL);
		llist->curr->xor = (xor_node_t*)XOR2(llist->prev, llist->next);
	}
	else {
		llist->next->xor = (xor_node_t*)XOR(llist->next->xor, 
										llist->curr, new_node);
		llist->curr->xor = (xor_node_t*)XOR(llist->curr->xor, 
										llist->next, new_node);
		new_node->xor = (xor_node_t*)XOR2(llist->curr, llist->next);
		llist->next = (xor_node_t*)new_node;
	}

	llist->size++;
}

void clear_freelist(xor_ll_t* llist) {
	xor_node_t* node;
	
	while (llist->freelist != NULL) {
		llist->destroy(llist->freelist->data);
		node = llist->freelist;
		llist->freelist = llist->freelist->xor;
		free(node);
	}

	llist->fsize = 0;
}

void remove_current_node(xor_ll_t* llist) {
	xor_node_t* removed_node;

	// if the list is empty, this function does nothing
	if (llist->curr == NULL)
		return;

	// storing the address of the node to be removed
	// so it can be put on the freelist
	removed_node = llist->curr;

	// removing pointers to current node
	if (llist->prev != NULL) {
		llist->prev->xor = (xor_node_t*)XOR(llist->prev->xor, 
											llist->curr, llist->next); 
		// adjusting the cursor
		llist->curr = llist->prev;
		llist->prev = (xor_node_t*)XOR2(llist->curr->xor, llist->next);
		llist->pos--;
	}
	else if (llist->next != NULL) {
		llist->next->xor = (xor_node_t*)XOR(llist->next->xor,
											llist->curr, llist->prev);
		// adjusting the cursor
		llist->curr = llist->next;
		llist->next = (xor_node_t*)XOR2(llist->curr->xor, llist->prev);
	}
	else {
		removed_node = llist->curr;
		llist->curr = NULL;
		llist->pos--;
	}
	
	// putting removed node on the freelist
	removed_node->xor = llist->freelist;
	llist->freelist = removed_node;
	llist->fsize++;

	// adjusting list size variable
	llist->size--;
}

int add_node_at_pos(xor_ll_t* llist, unsigned int pos) {
	if (pos < llist->size) {
		move_to_pos_xor_ll(llist, pos);
		add_node_b(llist);
		return 0;
	}
	else if (pos == llist->size) {
		move_to_pos_xor_ll(llist, llist->size - 1);
		add_node_f(llist);
		return 0;
	}
	else
		return -1;
}

// returns a node. Gets a node from the freelist, or allocates a new
// node if the freelist is empty
xor_node_t* get_node(xor_ll_t* llist) {
	xor_node_t* new_node;

	if (llist->freelist == NULL) {
		new_node = (xor_node_t*)malloc(sizeof(xor_node_t));
		new_node->data = llist->create();
	}
	else {
		new_node = llist->freelist;
		llist->freelist = llist->freelist->xor;
		llist->fsize--;
	}

	return new_node;
}

void print_xor_ll_info(xor_ll_t* llist) {
	printf("list size: %d\n", llist->size);
	printf("freelist size: %d\n", llist->fsize);
	printf("current position: %d\n\n", llist->pos);
}
