#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "xor_ll.h"

void* create_int();
void destroy_int(void*);

int main(int argc, char** argv) {
	xor_ll_t my_list;
	int i;
	unsigned int t;
	my_list = create_xor_ll(create_int, destroy_int);
	print_xor_ll_info(&my_list);

	// initializing pseudo-random number generator
	srand(time(NULL));

	// creating list
	for (i = 0; i < 1000; i++) 
		add_node_b(&my_list);
	print_xor_ll_info(&my_list);

	for (i = 0; i < 1000; i++)
		*((int*)my_list.curr->data) = rand();
	print_xor_ll_info(&my_list);
	
	for (i = 0; i < 1000; i++)
		remove_current_node(&my_list);
	print_xor_ll_info(&my_list);

	// clearing freelist
	clear_freelist(&my_list);
	print_xor_ll_info(&my_list);
}

void* create_int() {
	void* new_int;
	
	new_int = malloc(sizeof(int));
	
	return new_int;
}

void destroy_int(void* my_int) {
	free(my_int);
}
