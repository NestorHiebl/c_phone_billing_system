/**
 *      @file main.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The main procedure for the csv based phone billing project.
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
 * 
 *      
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "csv_to_linked_list.h"

/**
 *      @def Debug
 * 
 *      @brief initialize to start to program in debugging mode. 
 */
#define DEBUG

int main(int argc, char **argv){

    if (argc != 3) {
        fprintf(stderr, "Invalid argument count\n");
        return EXIT_FAILURE;
    }
    /**
    *       @property Total call number
    *       @brief The total number of calls in the given csv.
    */
    size_t total_call_number = 0;
    /**
    *       @property Total call duration
    *       @brief The total duration of calls in the given csv.
    */
    size_t total_call_duration = 0;
    /**
    *       @property Total call price
    *       @brief The total price of calls in the given csv.
    */
    double total_call_price = 0;

    // Push the first non-option argument into the call rate variable
    getopt(argc, argv, "--");
    FILE *call_rates = open_csv(optarg);

    // Push the second non-option argument into the call record variable
    getopt(argc, argv, "--");
    FILE *call_record = open_csv(optarg);

    if ((call_rates == NULL) || (call_record == NULL)) {
        fprintf(stderr, "Error loading files, aborting\n");
        return EXIT_FAILURE;
    }
    // Line 509842 in the csv has a great error example!

    rate_linked_list * rate_head = parse_rate_csv(call_rates);
    print_rate_list(rate_head, 0, 0);

    close_csv(call_rates);
    close_csv(call_record);

    #ifdef DEBUG

        printf("%li, %li, %f\n", total_call_number, total_call_duration, total_call_price);

        // Test for the rate linked list append, print and delete functions
        rate_linked_list *head = NULL;
        rate_linked_list *tail = NULL;

        append_rate(&head, &tail, "000", 0.5);
        append_rate(&head, &tail, "001", 0.5);
        append_rate(&head, &tail, "002", 0.5);
        append_rate(&head, &tail, "003", 0.5);
        append_rate(&head, &tail, "004", 0.5);
        append_rate(&head, &tail, "005", 0.5);
        append_rate(&head, &tail, "006", 0.5);

        print_rate_list(head, 0, 0);

        if (delete_rate_list(&head)) {
            printf("Success!\n");
        } else {
            printf("Failure!\n");
        }
        
        // print_rate_list(head, 0, 0);

        rate_node *rate_tree_root = NULL;

        rate_tree_root = add_rate_node(rate_tree_root, "01", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "02", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "05", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "04", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "03", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "06", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "07", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "13", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "10", 0.0);

        traverse_rates_inorder(rate_tree_root, print_rate_node);

    #endif



    return EXIT_SUCCESS;
}