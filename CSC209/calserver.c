#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lists.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/signal.h>
#include "wrapsock.h"

#define INPUT_BUFFER_SIZE 256
#define ARG_STRING_MAX_SIZE 256 
#define INPUT_ARG_MAX_NUM 8
#define DELIM " \n"
#define MAX_BACKLOG 10
#define TRUE 1
#define FALSE 0

//Keep track of the clients via a linked list, and make the listening socket global.
struct client *top = NULL;
int num_clients = 0;
int listenfd;

//Client representation. Taken from muffinman.c by Alan J. Rosenthal and modified.
struct client {
    int fd;
    int has_username;
    char username[INPUT_BUFFER_SIZE];
    char input_buffer[INPUT_BUFFER_SIZE];
    struct in_addr ipaddr;
    struct client *next;
}; 

//Functions to add and remove clients. Taken from muffinman.c by Alan J. Rosenthal and modified.
static struct client *addclient(int fd, struct in_addr addr)
{
    struct client *new_client = wrap_malloc(sizeof(struct client));
    fflush(stdout);
    new_client->fd = fd;
    new_client->ipaddr = addr;
//    printf("Adding client %s\n", inet_ntoa(addr));
    new_client->next = top;
    top = new_client;
    num_clients++;
    return new_client; //added last night
}


static void removeclient(int fd)
{
    struct client **p;
    for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next)
        ;
    if (*p) {
        struct client *t = (*p)->next;
        printf("Removing client %s\n", inet_ntoa((*p)->ipaddr));
        fflush(stdout);
        free(*p);
        *p = t;
        num_clients--;
    } else {
        fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
        fflush(stderr);
    }
}




void newconnection(void) {
    
   //Set up the peer for acceptance by the server.
   struct sockaddr_in peer_addr;
   socklen_t peer_size = sizeof(peer_addr);
   int connfd = Accept(listenfd, (struct sockaddr *) &peer_addr, &peer_size);

     //NOTE must replace confd with pointer to the newly created client.    
    //Start the client calendar session by querying the user for a name.
    char *username_query = "What is your username?\r\n";
    Write(connfd, username_query, strlen(username_query));
    struct client *new_client = addclient(connfd, peer_addr.sin_addr);
    new_client->has_username = FALSE;
}

int find_network_newline(char *buf, int inbuf) {

  int i = 0;
//  printf("inbuf parameter: %d\n", inbuf);
  while (i < inbuf && !(buf[i] == '\n')) {
//    printf("current i: %d\n", i);
  //  printf("searching for new line: %c\n ", buf[i]);
    i++;
  }

  if (buf[i] == '\n') {
    //printf("final i: %c\n", buf[i]);
    return i;
  }

  else {
    return -1;
  }
}


/* 
 * Print a formatted error message to stderr.
 */
void error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}


/* 
 * Return a calendar time representation of the time specified
 *  in local_args. 
 *    local_argv[0] must contain hour. 
 *    local_argv[1] may contain day, otherwise use current day
 *    local_argv[2] may contain month, otherwise use current month
 *    local_argv[3] may contain day, otherwise use current year
 */
time_t convert_time(int local_argc, char** local_argv) {

   time_t rawtime;
   struct tm * info;  

   // Instead of expicitly setting the time, use the current time and then
   // change some parts of it.
   time(&rawtime);            // get the time_t represention of the current time 
   info = localtime(&rawtime);   

   // The user must set the hour.
   info->tm_hour = atof(local_argv[0]);
   
   // We don't want the current minute or second. 
   info->tm_min = 0;
   info->tm_sec = 0;

   if (local_argc > 1) {
       info->tm_mday = atof(local_argv[1]);
   }
   if (local_argc > 2) {
       // months are represented counting from 0 but most people count from 1
       info->tm_mon = atof(local_argv[2])-1;
   }
   if (local_argc > 3) {
       // tm_year is the number of years after the epoch but users enter the year AD
       info->tm_year = atof(local_argv[3])-1900;
   }

   // start off by assuming we won't be in daylight savings time
   info->tm_isdst = 0;
   mktime(info);
   // need to correct if we guessed daylight savings time incorrect since
   // mktime changes info as well as returning a value. Check it out in gdb.
   if (info->tm_isdst == 1) {
       // this means we guessed wrong about daylight savings time so correct the hour
       info->tm_hour--;
   }
   return  mktime(info);
}


/* 
 * Read and process calendar commands
 * Return:  -1 for quit command
 *          0 otherwise
 */
int process_args(int cmd_argc, char **cmd_argv, Calendar **calendar_list_ptr, User **user_list_ptr, int client_fd) {

    Calendar *calendar_list = *calendar_list_ptr; 
    User *user_list = *user_list_ptr;
    struct client **p; 

    if (cmd_argc <= 0) {
        return 0;
    } else if (strcmp(cmd_argv[0], "quit") == 0 && cmd_argc == 1) {
        removeclient(client_fd);
	Close(client_fd);
        
    } else if (strcmp(cmd_argv[0], "add_calendar") == 0 && cmd_argc == 2) {
        if (add_calendar(calendar_list_ptr, cmd_argv[1]) == -1) {
	    char *calendar_error_notification = "Calendar by this name already exists\r\n";
            Write(client_fd, calendar_error_notification, strlen(calendar_error_notification));
        }
        
    } else if (strcmp(cmd_argv[0], "list_calendars") == 0 && cmd_argc == 1) {
       char  *cal_buf = list_calendars(calendar_list);
       Write(client_fd, cal_buf, strlen(cal_buf));
    
    } else if (strcmp(cmd_argv[0], "add_event") == 0 && cmd_argc >= 4) {
        // Parameters for convert_time are the values in the array
        // cmd_argv but starting at cmd_argv[3]. The first 3 are not
        // part of the time.
        // So subtract these 3 from the count and pass the pointer 
        // to element 3 (where the first is element 0).
        time_t time = convert_time(cmd_argc-3, &cmd_argv[3]);

        if (add_event(calendar_list, cmd_argv[1], cmd_argv[2], time) == -1) {
	   char *event_error_notification = "Calendar by this name does not exist\r\n"; 
           Write(client_fd, event_error_notification, strlen(event_error_notification));
        }

    } else if (strcmp(cmd_argv[0], "list_events") == 0 && cmd_argc == 2) {
        char *event_buf = list_events(calendar_list, cmd_argv[1]); //Error string is returned by lists.c when necessary.
	Write(client_fd, event_buf, strlen(event_buf));
	
    } else if (strcmp(cmd_argv[0], "add_user") == 0 && cmd_argc == 2) {
        if (add_user(user_list_ptr, cmd_argv[1]) == -1) {
		char *user_error_notification = "User by this name already exists\r\n";
                Write(client_fd, user_error_notification, strlen(user_error_notification));
        }
	else {
	     char *user_addition_notification = "Added user: ";
	     Write(client_fd, user_addition_notification, strlen(user_addition_notification));
	     Write(client_fd, cmd_argv[1], strlen(cmd_argv[1]));
	}

    } else if (strcmp(cmd_argv[0], "list_users") == 0 && cmd_argc == 1) {
        char *user_buf = list_users(user_list);
        Write(client_fd, user_buf, strlen(user_buf));

    } else if (strcmp(cmd_argv[0], "subscribe") == 0 && cmd_argc == 3) {
        int return_code = subscribe(user_list, calendar_list, cmd_argv[1], cmd_argv[2]);
        if (return_code == -1) {
	   char *user_error_message = "User by this name does not exist\r\n ";
           Write(client_fd, user_error_message, strlen(user_error_message));
        } else if (return_code == -2) {
	   char *calendar_error_message = "Calendar by this name does not exist\r\n ";
           Write(client_fd, calendar_error_message, strlen(calendar_error_message));
        } else if (return_code == -3) {
	   char *subscription_error_message = "This user is already subscribed to this calendar\r\n";
           Write(client_fd, subscription_error_message, strlen(subscription_error_message));
        } else {
		p = &top;
		while (*p) {
			 if (strncmp((*p)->username, cmd_argv[1], strlen(cmd_argv[1]) - 2) == 0) {
				char *subscription_notification = "You have been subscribed to the following calendar: ";
				Write((*p)->fd, subscription_notification, strlen(subscription_notification));
				Write((*p)->fd, cmd_argv[2], strlen(cmd_argv[2]));
			 }
			 p = &(*p)->next;
        	}
	
          }
      
    } else {
	char *syntax_message = "Incorrect syntax\r\n";
        Write(client_fd, syntax_message, strlen(syntax_message));
    }
    return 0;
}

int main(int argc, char* argv[]) {


    //Create empty heads for the calendar and list data  structures.   
    Calendar *calendar_list = NULL;
    User *user_list = NULL;


    // for holding arguments to individual commands passed to sub-procedure
    char *cmd_argv[INPUT_ARG_MAX_NUM];
    char arg_string[ARG_STRING_MAX_SIZE]; //May move this to be inside  serivce client function
    int cmd_argc;


    //Create a stream socket for use by the server for listening over the network.
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

   //Code so that the port in use will be released as soon as the server process terminates. 
    int on = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if(status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    //Set up the socket address struct for binding the listening sockt
    struct sockaddr_in listenfd_address; 
    memset(&listenfd_address, '\0', sizeof(listenfd_address));
    listenfd_address.sin_family = AF_INET;
    listenfd_address.sin_addr.s_addr = INADDR_ANY;
    listenfd_address.sin_port = htons(PORT);
    
    Bind(listenfd, (struct sockaddr *) &listenfd_address, sizeof(listenfd_address));
    Listen(listenfd, MAX_BACKLOG);
        
    while (1) {

	//Set up the select sets for multiplexing (taken from muffinman.c, and modified)
    	fd_set fdlist;
        int maxfd = listenfd;
        FD_ZERO(&fdlist);
        FD_SET(listenfd, &fdlist);
	struct client *cur_client;
	int nready = 0;
        for (cur_client = top; cur_client; cur_client = cur_client->next) {
            FD_SET(cur_client->fd, &fdlist);
            if (cur_client->fd > maxfd) {
                maxfd = cur_client->fd;
	    }
	}
	
	//Listen for user input or a new connection. (Taken for muffinman.c, and modified. 
        if ((nready = Select(maxfd + 1, &fdlist, NULL, NULL, NULL)) > 0) {
	    
	    for (cur_client = top; cur_client; cur_client = cur_client->next) {
                if (FD_ISSET(cur_client->fd, &fdlist)) {
		
		    if (cur_client->has_username == FALSE) {
			 Read(cur_client->fd, cur_client->username, INPUT_BUFFER_SIZE); 
   			 cur_client->has_username = TRUE;
    			 char *user_notification = "Go ahead and enter calendar commands\r\n";
    			 Write(cur_client->fd, user_notification, strlen(user_notification));
		    }

		    else {

		    //Set up variables to handle partial reads
    		    int inbuf = 0;
    		    int new_line_index = -1;
    		    int buffer_room = INPUT_BUFFER_SIZE;
    		    char *buffer_store_point = cur_client->input_buffer;
    		    //int amount_read;
    		    memset(cur_client->input_buffer,'\0', INPUT_BUFFER_SIZE); 

    		   while(new_line_index < 0) {

            		inbuf += Read(cur_client->fd, buffer_store_point, buffer_room);
	   		new_line_index = find_network_newline(cur_client->input_buffer, inbuf);
           
	   		//printf("in buffer after iteration: %s\n", input_buffer);
	   		//printf("current new line index: %d\n", new_line_index);
	   		if (new_line_index >= 0) {
	      			memset(arg_string,'\0', ARG_STRING_MAX_SIZE);
	      			strncpy(arg_string, cur_client->input_buffer, new_line_index - 1);
	      		//	printf("inbuf prior: %d\n", inbuf);
	      		//	printf("new_line_index + 1: %d\n", new_line_index + 1);
	      		//	printf("The buffer state: %s\n", cur_client->input_buffer);
	      			inbuf -= new_line_index + 1;
	      			buffer_room = INPUT_BUFFER_SIZE  - inbuf; 
			        if (inbuf >= 0) { //For some reason inbuf appears to take on a value of negative 1 at times. Looking on GDB showed a line without the \r, whihc likely set everything off
	      			    memmove(cur_client->input_buffer, &(cur_client->input_buffer[new_line_index + 2]), inbuf);
	      			    buffer_store_point = &(cur_client->input_buffer[inbuf]);
				    }
	      			break; //Since we must have new line, pass it to be processed.
	   		}

	   		else {
			    buffer_room = INPUT_BUFFER_SIZE - inbuf;
			    buffer_store_point = &(cur_client->input_buffer[inbuf]);
	   		}

       		   }

		 // printf("passed to arg string: %s\n", arg_string);
        	  char *next_token = strtok(arg_string, DELIM);
        	  cmd_argc = 0;
        	  while (next_token != NULL) {
            		if (cmd_argc >= INPUT_ARG_MAX_NUM - 1) {
                		error("Too many arguments!");
                		cmd_argc = 0;
                		break;
            		}
            		cmd_argv[cmd_argc] = next_token;
            		cmd_argc++;
            		next_token = strtok(NULL, DELIM);
        	 }
        	if (cmd_argc > 0 && process_args(cmd_argc, cmd_argv, &calendar_list, &user_list, cur_client->fd) == -1) {
            		break; // can only reach if quit command was entered
		}

	       }
	     }
	   }
		if (FD_ISSET(listenfd, &fdlist)) {
                    newconnection();
                }       
	}


    }
    return 0;
 }
