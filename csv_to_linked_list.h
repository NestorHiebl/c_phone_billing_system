/**
 *      @file csv_to_linked_list.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external function header for the csv based phone billing project 
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef CSV_TO_LINKED_LIST_FUNC

    #define CSV_TO_LINKED_LIST_FUNC

        typedef struct call_linked_list {
            
            char *caller;
            char *callee;

            int duration;

            char *datetime;

            call_linked_list *previous;
            call_linked_list *next;

        } call_linked_list;

        int open_csv(const char* filename);
        int close_csv(FILE);
    

#endif