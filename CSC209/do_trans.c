/* Read and execute a list of operations on a linked list.
 * Periodically call the garbage collector.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
//Since the file calls mark_and_sweep.
#include "gc.h"
//Since the file references mark_list.
#include "gc_list.h"
#include <signal.h>
#include <unistd.h>

#define MAX_LINE 128
#define ADD_NODE 1
#define DEL_NODE 2
#define PRINT_LIST 3

void SIGUSR1_handler(int signum);

//List head made global so that it can be used by the signal handler
 List *ll = NULL;
 //Global signal variable to use between files.
 sigset_t signal_to_block;


void SIGUSR1_handler(int signum) {
	mark_and_sweep(ll, mark_list);
}

int main(int argc, char **argv) {
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

    char line[MAX_LINE];
    char *str;
    int count = 0;

    if(argc != 2) {
        fprintf(stderr, "Usage: do_trans filename\n");
        exit(1);
    }

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen");
        exit(1);
    }


    while(fgets(line, MAX_LINE, fp) != NULL) {

        char *next;
        int value;
        int type = strtol(line, &next, 0);

        switch(type) {
            case ADD_NODE :
                value = strtol(next, NULL, 0);
                ll = add_node(ll, value);
                break;
            case DEL_NODE :
                value = strtol(next, NULL, 0);
                ll = remove_node(ll, value);
                break;
            case PRINT_LIST :
                str = tostring(ll);
                printf("List is %s\n", str);
                break;
            default :
                fprintf(stderr, "Error: bad transaction type\n");

        }



        if(count % 10 == 0) {
            mark_and_sweep(ll, mark_list);
            // You might want to add something here to
            // make your program pause long enough to see what it
            // is doing. In the commented out code, we wait for 
            // any user input before continuing.
 
            // char check[MAX_LINE];
            // fgets(check, MAX_LINE, stdin);
        }
        count++;

	if (count == 33) {
	kill(getpid(), SIGUSR1);
}
    }
    return 0;
}

