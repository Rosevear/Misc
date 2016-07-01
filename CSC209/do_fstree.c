#include <stdio.h>
#include <stdlib.h>
#include "fstree.h"
//Since the file calls mark_and_sweep.
#include "gc.h"
#include "gc_fstree.h"
#include <signal.h>

sigset_t signal_to_block;
extern Fstree *root;

void SIGUSR1_handler(int signum);

void SIGUSR1_handler(int signum) {
	mark_and_sweep(root, mark_fstree);
}

int main() {

	//Set up the SIGUSR1 struct.
	  struct sigaction custom_action;
	  custom_action.sa_handler = &SIGUSR1_handler;
	  custom_action.sa_flags = 0;
	  sigemptyset(&custom_action.sa_mask);

	  //Listen for SIGUSR1.
	  if (sigaction(SIGUSR1, &custom_action, NULL) != 0) {
			 perror("sigaction");
			 exit(1);
		}

	  //Initialize a signal set for use by sigprocmask.
	 if (sigemptyset(&signal_to_block) == -1) {
		 perror("sigemptyset");
		 exit(1);
	 }
	 if (sigaddset(&signal_to_block, SIGUSR1) == -1) {
			 perror("sigaddset");
			 exit(1);
	  }

    root = create_node("root");

//    do_transactions("fs_trans1");
  //   print_tree(root, 0);

    /* try uncommenting various bits to use other test files */

    do_transactions("fs_trans2");
    //print_tree(root, 0);

   // do_transactions("fs_trans_loop");
    // don't call print_tree once you have a loop. It isn't sophisticated 
    // enough to handle the loop


//      //To periodically call mark_and_sweep.
//      if (count_tree % 2 == 0) {
//    	  mark_and_sweep(root, mark_fstree);
//      }


/*
    Or you can put the calls directly in here but be careful, you
    can't use string literals for the paths or you will get Segmentation errors.

    add_node("","one");  // add_node (path, name)
    print_tree(root, 0);
    add_node("one","oneone");  // add_node (path, name)
    print_tree(root, 0);
    printf("************\n");

    char dest_path[20] = "one/oneone";
    char src_path[20] = "";
    char name[10] = "one";

    // this makes a link as a child of  one/oneone back to /one
    add_hard_link(dest_path, src_path, name);
    // DON"T call print_tree now since it doesn't handle loops and 
    //  will print infinitely
    // print_tree(root, 0);

*/

    // once you've implemented your garbage collector on fstree, you can
    // call it from here at various points to clean up the garbage
    mark_and_sweep(root, mark_fstree);

    return 0;
}
