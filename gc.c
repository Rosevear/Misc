#include <stdlib.h>
#include <stdio.h>
#include "gc.h"
#include <signal.h>
#include <unistd.h>

//Head of the memory reference list.
mem_chunk *mem_ref_list = NULL;
//Global variable to keep track of the length of the list in order to provide summary data.
int len = 0;
//Global variable to allow sigprocmask in list.c to block SIGUSR1.
sigset_t signal_to_block;

void add_mem_chunk(void *new_address) {
//Initialize a signal set for use by sigprocmask.

         if (sigemptyset(&signal_to_block) == -1) {
                 perror("sigemptyset");
                 exit(1);
         }
         if (sigaddset(&signal_to_block, SIGUSR1) == -1) {
                         perror("sigaddset");
                         exit(1);
          }

	mem_chunk *new_mem_chunk_ptr = NULL;
	if ((new_mem_chunk_ptr = malloc(sizeof(mem_chunk))) == NULL) {
		perror("malloc");
		exit(1);
	}

	//Add the new memory chunk to the head of the memory reference list,
	//making the old head point to the new memory chunk.
	new_mem_chunk_ptr->in_use = NOT_USED;
	new_mem_chunk_ptr->address = new_address;
	new_mem_chunk_ptr->next = mem_ref_list;

	//Block SIGUSR1 to prevent mark_and_sweep being called between now and
	//the addition of the node to its respective data structure.
	if (sigprocmask(SIG_BLOCK, &signal_to_block, NULL) == -1) {
	        perror("sigprocmask");
	        exit(1);
	 }
//To test that the signal is blocked at this point.
//	printf("No mark and sweep!\n");
//	usleep(1000000);
//	usleep(1000000);
//	usleep(1000000);
//	usleep(1000000);
//	usleep(1000000);
//	usleep(1000000);

	mem_ref_list = new_mem_chunk_ptr;
	len++;
}


void *gc_malloc(int nbytes) {

	void *new_ptr = NULL;
	if ((new_ptr = malloc(nbytes)) == NULL) {
		perror("malloc");
		exit(1);
	}

	add_mem_chunk(new_ptr);
	return new_ptr;
}


void mark_and_sweep(void *obj, void (*mark_obj)(void *)) {
	printf("MARK AND SWEEP!\n");
	//Open the logfile to append summary data.
	int old_len = len;
	FILE *log_file;
	if ((log_file = fopen(LOGFILE, "a")) == NULL) {
		perror("fopen");
		exit(1);
	}
	fprintf(log_file, "Engaging a new iteration of mark_and_sweep!\n");
	fprintf(log_file, "Current number of allocated memory chunks: %d\n", old_len);
	//Reset the memory reference list values to NOT_USED.
	mem_chunk *current_mem_chunk_ptr = mem_ref_list;
	while (current_mem_chunk_ptr != NULL) {
			current_mem_chunk_ptr->in_use = NOT_USED;
			current_mem_chunk_ptr = current_mem_chunk_ptr->next;
	}

	//Mark the data still in use by the given data structure.
	(*mark_obj)(obj);

	//Sweep through the memory reference list, deallocating the memory of
	//those addresses which can't be reached as well as the mem chunks that
	//are no longer needed
	mem_chunk *current_ptr = mem_ref_list;
	mem_chunk *temp = NULL;
	if (mem_ref_list != NULL) {
	  while (current_ptr->next != NULL) {
		if (current_ptr->next->in_use == NOT_USED) {
			free(current_ptr->next->address);
			temp = current_ptr->next;
			current_ptr->next = current_ptr->next->next;
			free(temp);
			len--;
//		printf("DEBUG: current ptr:%p   current_ptr->next: %p\n",current_ptr,  current_ptr->next);
		}
		else {
		current_ptr = current_ptr->next;
		}
	}
     }

	//Handle the edge case when the head of the memory reference list also needs to be removed.
//	if (mem_ref_list != NULL) {
//	  if (mem_ref_list->in_use == NOT_USED) {
//			temp = mem_ref_list;
//			mem_ref_list = mem_ref_list->next;
//			free(temp->address);
//			free(temp);
//		}
//	}
	//Write the summary data to the log file and close it.
	fprintf(log_file, "Amount of memory chunks freed this iteration: %d\n", old_len - len);
	if (fclose(log_file) != 0) {
		perror("fclose");
		exit(1);
	}
}

int mark_one(void *vptr) {
	mem_chunk *current_mem_chunk_ptr = mem_ref_list;
	while (current_mem_chunk_ptr->address != vptr) {
		current_mem_chunk_ptr = current_mem_chunk_ptr->next;
	}

	if (current_mem_chunk_ptr->in_use == NOT_USED) {
		current_mem_chunk_ptr->in_use = USED;
		return 0;
	}

	else {
		return 1;
	}
}


