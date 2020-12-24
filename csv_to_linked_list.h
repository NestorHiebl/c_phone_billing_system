/**
 *      @file csv_to_linked_list.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external function header for the csv based phone billing project. It saves the data in a double linked list with the following structure:
 * 
 *      user -> user -> user -> user -> user -> user -> user ->...
 *        |       |       |       |       |       |       | 
 *      call    call    call    call    call    call    call
 *        |       |       |       |       |       |       | 
 *      call    call    call    call    call    call    call
 *        |       |       |       |       |       |       | 
 *      call    call    call    call    call    call    call
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CSV_TO_LINKED_LIST_FUNC

    #define CSV_TO_LINKED_LIST_FUNC

        /**
         *      @typedef Rate linked list
         * 
         *      @brief The call rate node. Only used to store rate information. The data inside this structure will not be changed after 
         *      being loaded in.
         * 
         *      @param extension The number extension, formatted as a string to make longest match searches easier.
         *      @param region The region descriptor in string format. Has no practical function as of yet.
         *      @param rate The call rate in double format. Determines the cost of a call to the extension per minute.
         * 
         *      @param previous The previous node. NULL for the head node.
         *      @param next The next node. NULL for the tail node.      
         */
        typedef struct rate_linked_list {
            
            char *extension;

            char *region;

            double rate;

            call_linked_list *previous;
            call_linked_list *next;

        } call_linked_list;

        /**
         *      @typedef User list node
         * 
         *      @brief The user node. It contains the head of another linked list with all of the respective user's calls.
         *      The rest of the node's fields are populated based on this list.
         * 
         *      @param number The user's unique number in string format. Used as the sole identifier for the user.
         *      @param call_list_head The head of the user's full list of calls.
         * 
         *      @param total_call_number Total number of calls the user has made. Only used for final stat calculation.
         *      @param total_call_duration Total duration the user's calls. Only used for final stat calculation.
         *      @param total_bill The user's total phone bill. Only used for final stat calculation.
         * 
         *      @param previous The previous node. NULL for the head node.
         *      @param next The next node. NULL for the tail node.      
         */
        typedef struct user_list {
            
            char *number;

            user_call_list *call_list_head;

            size_t total_call_number;
            size_t total_call_duration;
            double total_bill;

            user_list *previous;
            user_list *next;

        } user_list;

        /**
         *      @typedef Call linked list
         * 
         *      @brief The node for a single call.
         * 
         *      @param callee The number that was called in string format, with the final 3 digits replaced with '*'.
         *      @param duration The duration of the call.
         *      @param price The call price in double format. Calculated from the duration and the appropriate node in the rate linked list.
         * 
         *      @param year The year the call was made in.
         *      @param month The month the call was made in.
         * 
         *      @param previous The previous node. NULL for the head node.
         *      @param next The next node. NULL for the tail node.      
         */        
        typedef struct user_call_list {
            
            char *callee;
            size_t duration;
            double price;

            size_t year;
            size_t month;

            user_call_list  *previous;
            user_call_list *next;

        } user_call_list;
        

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
        int append_user(user_list **head, user_list **tail, char *number);
        void print_bill_list(user_list *head);
        void print_bill_list_slice(user_list *head, int starting_index, int ending_index);
        int delete_bill_list(user_list **head);

        int generate_bill_file(user_list *bill);
        int generate_cdr_file(user_list *bill);

#endif