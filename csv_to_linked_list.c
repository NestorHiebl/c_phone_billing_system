/**
 *      @file csv_to_linked_list.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external functions for the csv based phone billing project 
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
*/

#include <stdio.h>
#include <stdlib.h>
#include "csv_to_linked_list.h"

/**
 *      Open CSV
 * 
 *      @brief Handles opening a csv file with a given name.
 * 
 *      @param filename The filename of the csv file.
 * 
 *      @returns The file pointer to given file.
 *      @retval NULL If the file is not found or if handling has failed 
 */
FILE *open_csv(const char* filename){
    size_t filename_len = strlen(filename);
    
    // Checks if the passed filename ends in ".csv"
    if ((strcmp(&filename[filename_len - 4], ".csv")) != 0) {
        fprintf(stderr, "Please provide a csv file\n");
        return NULL;
    }

    FILE *csv = fopen(filename, "r");

    if (csv == NULL) {
        printf("Filename invalid, aborting\n");
        return NULL;
    }
    
    return csv;
}

/**
 *      Close CSV
 * 
 *      @brief Handles closing a csv file pointer.
 * 
 *      @param filename The pointer to csv file.
 * 
 *      @returns An integer indicator of the function's success
 *      @retval 0 If file closing failed
 *      @retval 1 If file closing was successful
 */
int close_csv(FILE *filepointer) {

    if (fflush(filepointer) != 0) {
        fprintf(stderr, "File flush failed\n");
        return 0;
    }

    if (fclose(filepointer) != 0) {
        fprintf(stderr, "File closing failed\n");
        return 0;
    }

    return 1;
}

/**
 *      Parse rate csv
 * 
 *      The iterative logic for parsing rows and fields is based on @c fgets and @c strtok , respectfully. Potential error scenarions are:
 *      @li A row in the csv is longer than 1024 characters. The function will notify you, but won't attempt to salvage the row.
 *      @li A field is missing. Strtok ignores consecutive delimiters, so any rows with NaN fields will be discarded.
 * 
 *      @brief Builds a full rate linked list based on a csv file pointer
 *      
 *      @param filename The @c FILE pointer for the csv
 * 
 *      @returns A pointer to the head of the generated linked list. Note that no tail is provided.
 */
rate_linked_list *parse_rate_csv(FILE *filename) {
    
    // rate_linked_list *head = NULL;

    char csv_line[1024];

    rate_linked_list *head = NULL;
    rate_linked_list *tail = NULL;

    // Used for debugging
    size_t line_counter = 0;
    while (!(feof(filename))) {
        if ((fgets(csv_line, 1024, filename)) != NULL) {
            
            // Going smoothly, the line has been loaded in successfuly

            if (((csv_line[strlen(csv_line) - 1]) == '\n')) {
                printf("Line ended\n");
                
            } else if (feof(filename)) {
                // We've reached the end of the file
                printf("File ended\n");    
            } else {
                // We're dealing with a really long line
                printf("Line longer than 1024 characters\n");
                continue;  
            } 
                
            char *extension_token = strtok(csv_line, ",");
            if (extension_token == NULL) {
                fprintf(stderr, "Line %lu is empty\n", line_counter);
                continue;
            }
            printf("%s ", extension_token);
            
            char *region_token = strtok(NULL, ",");
            if (region_token == NULL) {
                fprintf(stderr, "Line %lu is missing two arguments\n", line_counter);
                continue;
            }
            printf("%s ", region_token);

            char *rate_token = strtok(NULL, ",");
            if (rate_token == NULL) {
                fprintf(stderr, "Line %lu is missing one argument\n", line_counter);
                continue;
            }
            printf("%s ", rate_token);
            
            if (strtok(NULL, ",") != NULL) {
                fprintf(stderr, "Additional field found on line %lu\n", line_counter);
                continue;
            }

            extension_token = validate_extension(extension_token);
            double rate = validate_rate(rate_token);   
            
            if ((extension_token != NULL) && rate) {
                // At this point, the node is ready for creation
                append_rate(&head, &tail, extension_token, rate);
            } else {
                printf("Invalid extension or rate found on line %lu\n", line_counter);
                continue;
            }
            

        } else {
            // Couldn't load a line in
            fprintf(stderr, "Loading line %lu in csv file failed, aborting\n", line_counter);
            return head;    
        }
        line_counter++;
    }
    return head;
}

char *validate_extension(char *extension){
    printf("%s\n", extension);
    return NULL;
}


double validate_rate(char *rate){
    printf("%s\n", rate);
    return 0.0;
}

/*****************************************************************************************************************
 * RATE LINKED LIST FUNCTIONS                                                                                    *
 *****************************************************************************************************************/

/**
 *      Appent rate node
 * 
 *      @brief Appends a new node to the rate linked list. If the head argument is NULL, it initializes a list instead.
 *      
 *      @param head A double pointer to the head of the list, which will be changed dynamically.
 *      @param tail A double pointer to the tail of the list, which will be changed dynamically.
 *      @param extension The extension string, cannot be NULL.
 *      @param rate The rate associated with the extension.
 * 
 *      @returns 1 if the function suceeds, 0 if it fails.
 */
int append_rate(rate_linked_list **head, rate_linked_list **tail, char *extension, double rate) {

    if (extension == NULL) {
        fprintf(stderr, "Extension string empty, aborting\n");
        return 0;
    } else if (!(*head == NULL) && ((*tail)->next) != NULL) {
        fprintf(stderr, "Head node is not last node, aborting\n");
        return 0;
    }
    
    rate_linked_list *new_node = malloc(sizeof(rate_linked_list));
    if (new_node == NULL) {
        fprintf(stderr, "Not enough memory to create new node\n");
        return 0;
    }

    // Initialize extension
    new_node->extension = malloc(sizeof(extension));
    if (new_node->extension == NULL) {
        fprintf(stderr, "Not enough memory to initialize extension\n");
        return 0;
    } else {
        strcpy(new_node->extension, extension);    
    }
    
    new_node->rate = rate;
    
    if (*head == NULL) {
        // The list is being initialized

        new_node->previous = NULL;
        new_node->next = NULL;

        *head = new_node;
        *tail = new_node;
    } else {
        // The node is being appended to an existing list
        (*tail)->next = new_node;

        new_node->previous = *tail;
        new_node->next = NULL;

        *tail = new_node;

        printf("Appended!\n");
    }
    
    return 1;    
}

/**
 *      Print rate list
 * 
 *      @brief Prints a slice of a rate linked list. If both indexes are set to 0, prints the entire list instead.
 * 
 *      @param head The head of the linked list.
 *      @param start_index The starting index of the list slice.
 *      @param end_index The ending index of the list slice.
 */
void print_rate_list(rate_linked_list *head, size_t start_index, size_t end_index) {
    if (head == NULL) {
        fprintf(stderr, "Cannot print NULL list, aborting\n");
        return;
    } else if (start_index > end_index) {
        fprintf(stderr, "Starting index larger than ending index, aborting");
        return;
    }
    
    
    rate_linked_list *current = head;
    if ((start_index == 0) && (end_index == 0)) {
        // If no start and end indexes were given print the whole list

        while (current != NULL) {
        printf("The rate for the extension \"%s\" equals %f.\n", current->extension, current->rate);
        current = current->next;
        }    
    } else {
        // Print list from start index to end index
        size_t i = 0;

        while (current != NULL) {
            if (i >= start_index) {
                // If we've reached the start index, print the node
                printf("The rate for the extension \"%s\" equals %2f.\n", current->extension, current->rate);
            }

            if (i > end_index) {
                // If we've exceeded the end index, exit the while loop
                break;
            }
            i++;
            current = current->next;
        }
    }
    return;   
}


int delete_rate_list(rate_linked_list **head) {
    if (*head == NULL) {
        fprintf(stderr, "Cannot delete NULL list, aborting\n");
        return 0;
    }

    rate_linked_list *current = *head;

    while (*head != NULL) {
        current = *head;

        free(current->extension);                       
        current->extension = NULL;

        *head = (*head)->next;
        free(current);
        current = NULL;
    }
    
    return 1;
}

/*****************************************************************************************************************
 * USER LINKED LIST FUNCTIONS                                                                                    *
 *****************************************************************************************************************/

/**
 *      Appent user node
 * 
 *      @brief Appends a new node to the user linked list. If the head argument is NULL, it initializes a list instead.
 *      Total call duration, number and price are initialized at 0. Call list head is initialized as NULL.
 *      
 *      @param head A double pointer to the head of the list, which will be changed dynamically.
 *      @param tail A double pointer to the tail of the list, which will be changed dynamically.
 *      @param number The users number, which will be used as their sole identifier.
 * 
 *      @returns 1 if the function suceeds, 0 if it fails.
 */
int append_user(user_list **head, user_list **tail, char *number) {

    if (number == NULL) {
        fprintf(stderr, "Number string empty, aborting\n");
        return 0;
    } else if (!(*head == NULL) && ((*tail)->next) != NULL) {
        fprintf(stderr, "Head node is not last node, aborting\n");
        return 0;
    }
    
    user_list *new_node = malloc(sizeof(user_list));
    if (new_node == NULL) {
        fprintf(stderr, "Not enough memory to create new user node\n");
        return 0;
    }

    // Initialize number
    new_node->number = malloc(sizeof(number));
    if (new_node->number == NULL) {
        fprintf(stderr, "Not enough memory to initialize number\n");
        return 0;
    } else {
        strcpy(new_node->number, number);    
    }
    
    new_node->total_bill = 0;
    new_node->total_call_duration = 0;
    new_node->total_call_number = 0;

    new_node->call_list_head = NULL;
    
    if (*head == NULL) {
        // The list is being initialized

        new_node->previous = NULL;
        new_node->next = NULL;

        *head = new_node;
        *tail = new_node;
    } else {
        // The node is being appended to an existing list
        (*tail)->next = new_node;

        new_node->previous = *tail;
        new_node->next = NULL;

        *tail = new_node;
    }
    
    return 1;    
}

// Note - The User list deletition function needs to be nested, because each node contains its own linked list.