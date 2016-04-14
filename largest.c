#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* MAXIMUM ARRAY LENGTH JUSTIFICATION
Since the names of things can only be 32 characters, and the length of the
size of files is always small enough to be contained by an int, the length
of any given line should be within 1000 characters.
*/

#define MAX_LINE_LENGTH 1000
#define MAX_NAME_LENGTH 32
#define FILE_FLAG 0
#define GROUP_READABLE_FLAG 4
#define FILE_GROUP_NAME_FLAG 3
#define FILE_SIZE_FLAG 4
#define FILE_NAME_FLAG 8
#define MAX_FILE_SIZE_DIGITS 10
#define TRUE 1
#define FALSE 0


int main(int argc, char **argv) {

    char current_line[MAX_LINE_LENGTH] = "";
    char previous_line[MAX_LINE_LENGTH] = "";

    //Skip the first line since it is not a file or directory.
    gets(current_line);

    //Declare and initialize some tracker variables
    char current_file_size[MAX_FILE_SIZE_DIGITS];
    int current_file_size_int;
    char current_line_group_name[MAX_NAME_LENGTH] = "";
    int current_largest_file_size = 0;
    char current_largest_file_name[MAX_NAME_LENGTH] = "";
    int first_file_found = FALSE;

    //Engage the main loop that reads from standard input
    while (TRUE == 1) {

    	/*If the current and previous lines are exactly the same, then since
    	 * file names are always different, the loop must be repeating, and
    	 * the program has reached the end of standard input. So terminate it.
    	 */
    	strcpy(previous_line, current_line);
    	gets(current_line);
    	if (strcmp(previous_line, current_line) == 0) {
    	    break;
    	}

		int current_line_length = strlen(current_line);

		//Analyze the line only if it is a group readable file, else skip it.
		if ((current_line[FILE_FLAG] == '-')
				&& (current_line[GROUP_READABLE_FLAG] == 'r')) {

			int current_line_index = 0;
			int section_signifier = 0;
			while (current_line_index < current_line_length) {

				//Uses the first white space characters encountered as flags to
				//identify where the index is in the current line.
				if (current_line[current_line_index] == ' ') {
					section_signifier++;

					//Loop over the remaining white spaces after the initial
					// section signifier to reach the data of that section.
					while (current_line[current_line_index + 1] == ' ') {
						current_line_index++;
					}
				}

				/*
				This path signifies the existence of optional arguments,
				and that the current line index has reached the group name
				substring within the line.
				*/
				if ((argc != 1) && (section_signifier == FILE_GROUP_NAME_FLAG)){


					//Clear the group name array, and reset its index counter
					current_line_index++;
					int current_line_group_name_index = 0;
					memset(&current_line_group_name[0], 0,
							sizeof(current_line_group_name));

					//Extract and store the group name substring.
					while (current_line[current_line_index] != ' ') {
						current_line_group_name[current_line_group_name_index]
					    = current_line[current_line_index];
						current_line_group_name_index++;
						current_line_index++;
					}

					//Skip the rest of the line if the group name does not
					//match the user supplied optional argument.
					if ((strlen(current_line_group_name) > strlen(argv[2])) ||
							(strncmp(current_line_group_name, argv[2],
									strlen(argv[2])) != 0)) {
						break;
					}

				}

				/*Taking this path signifies that the line analyzer has reached
				 * the file size section of the line.
				 */
				if (section_signifier == FILE_SIZE_FLAG) {

					//Reset the file size array container and its index
					current_line_index++;
					int current_file_size_index = 0;
					memset(&current_file_size[0], 0, sizeof(current_file_size));

					//Loop over all the file size digits in the line,
					//and store them for analysis.
					while (current_line[current_line_index] != ' ') {
						current_file_size[current_file_size_index]
						    = current_line[current_line_index];
						current_file_size_index++;
						current_line_index++;
					}

					current_file_size_int = atoi(current_file_size);

					//Update the current largest file if necessary and find the
					//corresponding file name.
					if ((first_file_found == FALSE)
							|| (current_largest_file_size
									< current_file_size_int)) {

						current_largest_file_size = current_file_size_int;
						first_file_found = TRUE;

						//Loop through the rest of the line in order to find
						//the file name
						int current_largest_file_name_index = 0;

						memset(&current_largest_file_name[0], 0,
								sizeof(current_largest_file_name));

						while (current_line[current_line_index] != '\0') {

							if (current_line[current_line_index] == ' ') {

								section_signifier++;
							}

							current_line_index++;


							//Extract and store the file name substring
							if (section_signifier == FILE_NAME_FLAG) {
								current_largest_file_name
								    [current_largest_file_name_index]
								        = current_line[current_line_index];

								current_largest_file_name_index++;
							}

						}

					}
					//Skip finding the current file name substring if the
					//current file size is too small.
					else {
						break;
					}
				}

				//If no white spaces found, simply check the next character
				else {
					current_line_index++;
				}

			}
		}
	}

	printf("%s\t%d\n", current_largest_file_name, current_largest_file_size);
	return 0;
}
