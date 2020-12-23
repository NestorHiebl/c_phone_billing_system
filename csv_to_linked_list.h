/**
 *      @file csv_to_linked_list.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external function header for the csv based phone billing project. It saves the data in a number of  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CSV_TO_LINKED_LIST_FUNC

    #define CSV_TO_LINKED_LIST_FUNC

        typedef struct rate_linked_list {
            
            char *extension;

            char *region;

            double rate;

            call_linked_list *previous;
            call_linked_list *next;

        } call_linked_list;

        typedef struct caller_monthly_bill {
            
            char *number;

            size_t year;
            size_t month;

            user_monthly_call_list *call_list_head;

            size_t total_call_number;
            size_t total_call_duration;
            size_t total_bill;

            caller_monthly_bill *previous;
            caller_monthly_bill *next;

        } caller_monthly_bill;
        
        typedef struct user_monthly_call_list {
            
            char *callee;

            size_t duration;

            double price;

            user_monthly_call_list  *previous;
            user_monthly_call_list *next;

        } user_monthly_call_list;
        

        // Functions for file handling
        int open_csv(const char* filename);
        int close_csv(FILE);
    
        // Pattern checking functions
        int validate_phone_number(char *phone_number);
        int validate_duration(int duration);

        // Rate linked list functions
        int append_rate_by_value(call_linked_list **head, call_linked_list **tail, char *extension, char *region, double rate);
        call_linked_list *find_rate_by_extension(call_linked_list *head, char *extension);
        void print_rate_list(call_linked_list *head);
        void print_rate_list_slice(call_linked_list *head, int starting_index, int ending_index);
        int delete_rate_list(call_linked_list **head);

        // Bill linked list functions
        int append_bill_by_value(caller_monthly_bill **head, caller_monthly_bill **tail, char *number, size_t year, size_t month);
        void print_bill_list(caller_monthly_bill *head);
        void print_bill_list_slice(caller_monthly_bill *head, int starting_index, int ending_index);
        int delete_bill_list(caller_monthly_bill **head);

        int generate_bill_file(caller_monthly_bill *bill);
        int generate_cdr_file(caller_monthly_bill *bill);

#endif