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
    return NULL;
}


double validate_rate(char *rate){
    return 0.0;
}

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
    
        new_node->previous = *tail;
        new_node->next = NULL;

        *tail = new_node;
    }
    
    return 1;

    
}