/**
 *      @headerfile csv_to_linked_list.h ""
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
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef CSV_TO_LINKED_LIST_FUNC

    #define CSV_TO_LINKED_LIST_FUNC

        /**
         *      @typedef Rate linked list
         * 
         *      @brief The call rate node. Only used to store rate information. The data inside this structure will not be changed after 
         *      being loaded in.
         * 
         *      @param extension The number extension, formatted as a @c string to make longest match searches easier.
         *      @param rate The call rate in @c double format. Determines the cost of a call to the extension per minute.
         * 
         *      @param previous The previous node. @c NULL for the head node.
         *      @param next The next node. @c NULL for the tail node.      
         */
        typedef struct rate_linked_list {
            
            char *extension;
            double rate;

            struct rate_linked_list *previous;
            struct rate_linked_list *next;

        } rate_linked_list;

        /**
         *      @typedef Call linked list
         * 
         *      @brief The node for a single call.
         * 
         *      @param callee The number that was called in @c string format, with the final 3 digits replaced with '*'.
         *      @param duration The duration of the call.
         *      @param price The call price in @c double format. Calculated from the duration and the appropriate node in the rate linked list.
         * 
         *      @param year The year the call was made in.
         *      @param month The month the call was made in.
         * 
         *      @param previous The previous node. @c NULL for the head node.
         *      @param next The next node. @c NULL for the tail node.      
         */        
        typedef struct user_call_list {
            
            char *callee;
            size_t duration;
            double price;

            size_t year;
            size_t month;

            struct user_call_list  *previous;
            struct user_call_list *next;

        } user_call_list;

        /**
         *      @typedef User list node
         * 
         *      @brief The user node. It contains the head of another linked list with all of the respective user's calls.
         *      The rest of the node's fields are populated based on this list.
         * 
         *      @param number The user's unique number in @c string format. Used as the sole identifier for the user.
         *      @param call_list_head The head of the user's full list of calls.
         * 
         *      @param total_call_number Total number of calls the user has made. Only used for final stat calculation.
         *      @param total_call_duration Total duration the user's calls. Only used for final stat calculation.
         *      @param total_bill The user's total phone bill. Only used for final stat calculation.
         * 
         *      @param previous The previous node. @c NULL for the head node.
         *      @param next The next node. @c NULL for the tail node.      
         */
        typedef struct user_list {
            
            char *number;

            user_call_list *call_list_head;

            size_t total_call_number;
            size_t total_call_duration;
            double total_bill;

            struct user_list *previous;
            struct user_list *next;

        } user_list;
        
        // Functions for file handling

        FILE *open_csv(const char* filename);
        int close_csv(FILE *filepointer);

        int parse_rate_csv(FILE *filename);
        int parse_call_csv(FILE *filename);
    
        // Pattern checking functions

        int validate_phone_number(char *phone_number);
        int validate_duration(int duration);

        // Rate linked list functions

        int append_rate(rate_linked_list **head, rate_linked_list **tail, char *extension, double rate);
        void print_rate_list(rate_linked_list *head);
        void print_rate_list_slice(rate_linked_list *head, size_t starting_index, size_t ending_index);
        rate_linked_list *search_by_longest_extension_match(rate_linked_list *head, const char *exension);
        int delete_rate_list(rate_linked_list **head);

        // User linked list functions
        
        int append_user(user_list **head, user_list **tail, char *number);
        void print_user_list(user_list *head);
        void print_user_list_slice(user_list *head, size_t starting_index, size_t ending_index);
        int delete_user_list(user_list **head);

        int add_user_call(user_list *user, const char *callee, size_t duration, double price, size_t year, size_t month);

        int calculate_user_stats(user_list *user);

        int generate_monthly_bill_files(user_list *bill);
        int generate_monthly_cdr_files(user_list *bill);

#endif