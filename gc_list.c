#include <stdio.h>
#include "list.h"
#include "gc.h"
#include "gc_list.h"

void mark_list(void *list_root) {
	List *current_node_ptr = list_root;
	while (current_node_ptr != NULL) {
		mark_one(current_node_ptr);
		current_node_ptr = current_node_ptr->next;
	}
}
