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


rate_linked_list *parse_rate_csv(FILE *filename) {
    
    // rate_linked_list *head = NULL;

    char csv_line[1024];

    while (!(feof(filename))) {
        if ((fgets(csv_line, 1024, filename)) != NULL) {
            if ((csv_line[strlen(csv_line) - 1]) == '\n') {
                // Going smoothly, the line has been loaded in successfuly
                printf("Line terminates in a newline!\n");

                char *field = NULL;
                size_t field_counter = 0;

                field = strsep(&csv_line, ",");
                printf("%s ", field);

                while ((field = strsep(&csv_line, ",")) != NULL) {
                    printf("%s ", field);
                }
                
                printf("\n");

            } else if (feof(filename)) {
                // We've reached the end of the file
                printf("File ended\n");    
            } else {
                // We're dealing with a really long line
                printf("Line longer than 1024 characters\n");  
            } 
        } else {
            // We couldn't load a line in
            fprintf(stderr, "Loading line in csv file failed, aborting\n");
            return NULL;    
        }
    }
    return NULL;
}

char **strsep_custom(char **stringp) {
    
    if (*stringp == NULL) {
        return NULL;
    }
    
    char **retval = malloc(sizeof(char*));
    if (retval == NULL) {
        fprintf(stderr, "Error allocating space for strsep_custom return string\n");
        return NULL;
    }
    

    char *current = *stringp;

    char comma = ',';

    while (strchr(current, comma) != NULL) {
        
        // Placeholder var to remember the starting position of the current token
        char *token = current;

        // Move the current pointer to the next comma
        current = strchr(current, comma);

        // Replace the comma with a string terminator
        *current = '\0';

        // Allocate space for the current token
        *retval = malloc(sizeof(char) * strlen(token));
        strcpy(retval, token);
    }

}