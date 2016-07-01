#include <stdio.h>
#include "gc.h"
#include "gc_fstree.h"
#include "fstree.h"
extern Fstree* root;

void mark_fstree(void *tree_root) {

	//To prevent mark_one from being re-run on the newly marked node at each recursive call.
	if (tree_root == root) {
		mark_one(tree_root);
	}

	Fstree *current_root = tree_root;
	Link *current_link = current_root->links;
	while (current_link != NULL) {
		if (mark_one(current_link->fptr) == 0) {
			mark_fstree(current_link->fptr);
		}
		current_link = current_link->next;
	}
}
