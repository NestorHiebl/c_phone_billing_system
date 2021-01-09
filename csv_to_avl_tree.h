/**
 *      @headerfile csv_to_avl_tree.h ""
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external function header for the csv based phone billing project. It saves rate data in an AVL tree and user data in an
 *      AVL tree where each node is the head of a linked list containing the respective user's call data. Data is collected by parsing two
 *      csv files using @c fgets and @c strtok. Invalid or corrupt data is logged and discarded with no attempt at recovery.
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef CSV_TO_AVL_TREE_FUNC
    #define CSV_TO_AVL_TREE_FUNC

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
         *      @typedef Rate tree node
         * 
         *      @brief The call rate tree node. Used to store rate information. The data inside this structure will not be changed after 
         *      being loaded in. Note that there is no reference to the parent node because the tree is static after the data has been
         *      loaded in. No deletitions or other complex operations will take place.
         * 
         *      @param region_code The number region code, formatted as a @c string to make longest match searches easier.
         *      @param rate The call rate in @c double format. Determines the cost of a call to the region code per minute.
         * 
         *      @param left The left child node.
         *      @param next The right child node.      
         */
        typedef struct rate_node {

            char *region_code;
            double rate;

            int height;

            struct rate_node *left;
            struct rate_node *right;
        } rate_node;

        /**
         *      @typedef User tree node
         * 
         *      @brief The user node. It contains the head of a linked list with all of the respective user's calls.
         * 
         *      @param number The user's unique number in @c string format. Used as the sole identifier for the user.
         *      @param call_list_head The head of the user's full list of calls.
         * 
         *      @param total_call_number Total number of calls the user has made. Only used for final stat calculation.
         *      @param total_call_duration Total duration the user's calls. Only used for final stat calculation.
         *      @param total_bill The user's total phone bill. Only used for final stat calculation.
         * 
         *      @param height The height of the node. Used to calculate balance, updated automatically by the rebalance function.
         * 
         *      @param parent The parent node. @c NULL for the root node.
         *      @param left The left child node.
         *      @param right The right child node.    
         */
        typedef struct user_node {
            
            char *number;

            user_call_list *call_list_head;

            size_t total_call_number;
            size_t total_call_duration;
            double total_bill;

            int height;

            struct user_node *left;
            struct user_node *right;

        } user_node;
        
        // Functions for file handling

        FILE *open_csv(const char* filename);
        int close_csv(FILE *filepointer);

        rate_node *parse_rate_csv(FILE *filename);
        user_node *parse_call_csv(FILE *filename);
    
        // Pattern checking functions

        char *validate_phone_number(char **phone_number);
        char *validate_region_code(char **region_code);
        char *validate_rate(char *rate);

        rate_node *search_by_longest_region_code_match(rate_node *root, const char *callee_number);
        
        char *censor_calee_numer(char *callee_number);

        int max(int a, int b);

        // Call linked list functions

        int insert_call(user_call_list **head, user_call_list **tail, char *callee_number, size_t duration, size_t year, size_t month, rate_node *rate_root);
        void print_call_list(user_call_list *head, size_t start_index, size_t end_index);
        int delete_call_list(user_call_list **head);

        // Rate AVL Tree functions

        rate_node *add_rate_node(rate_node *node, const char *region_code, double rate);
        rate_node *make_rate_node(const char *region_code, double rate);

        int get_rate_node_height(rate_node *node);
        int get_rate_node_balance(rate_node *node);

        rate_node *left_rotate_rate(rate_node *node);
        rate_node *right_rotate_rate(rate_node *node);

        void traverse_rates_inorder(rate_node *node, void (*visit) (rate_node*));
        void traverse_rates_postorder(rate_node *node, void (*visit) (rate_node*));
        void print_rate_node(rate_node *node);
        void delete_rate_node(rate_node *node);
        
        // User AVL Tree functions

        user_node *add_user_node(user_node *node, const char *caller_number, const char *callee_number, size_t duration, size_t year, size_t month);
        user_node *make_user_node(const char *number);
        
        int get_user_node_height(user_node *node);
        int get_user_node_balance(user_node *node);

        user_node *left_rotate_user(user_node *node);
        user_node *right_rotate_user(user_node *node);

        int add_user_call(user_node *user, const char *callee, size_t duration, double price, size_t year, size_t month);

        void traverse_users_inorder(user_node *node, void (*visit) (user_node*));
        void traverse_users_postorder(user_node *node, void (*visit) (user_node*));
        void print_user_node(user_node *node);
        void delete_user_node(user_node *node);
        void calculate_user_stats(user_node *user);
        void generate_monthly_bill_files(user_node *bill);
        void generate_monthly_cdr_files(user_node *bill);

#endif