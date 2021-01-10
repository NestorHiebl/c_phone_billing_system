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
#include "csv_to_avl_tree.h"

/**
 *      @def Debug
 * 
 *      @brief initialize to start the program in debugging mode. 
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

    rate_node *rate_root = parse_rate_csv(call_rates);
    traverse_rates_inorder(rate_root, print_rate_node);

    close_csv(call_rates);
    close_csv(call_record);

    traverse_rates_postorder(rate_root, delete_rate_node);
    rate_root = NULL;

    #ifdef DEBUG

        printf("%li, %li, %f\n", total_call_number, total_call_duration, total_call_price);


        rate_node *rate_tree_root = NULL;

        rate_tree_root = add_rate_node(rate_tree_root, "01", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "02", 5.0);
        rate_tree_root = add_rate_node(rate_tree_root, "05", 5.0);
        rate_tree_root = add_rate_node(rate_tree_root, "04", 5.0);
        rate_tree_root = add_rate_node(rate_tree_root, "03", 5.0);
        rate_tree_root = add_rate_node(rate_tree_root, "06", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "07", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "13", 0.0);
        rate_tree_root = add_rate_node(rate_tree_root, "10", 0.0);

        traverse_rates_inorder(rate_tree_root, print_rate_node);

        user_call_list *head = NULL;

        insert_call(&head, "436802119876", 4, 2000, 10, rate_root);
        insert_call(&head, "436642129876", 4, 2010, 11, rate_root);
        insert_call(&head, "436504069876", 4, 2009, 6, rate_root);
        insert_call(&head, "4369910149876", 4, 2020, 5, rate_root);
        insert_call(&head, "431311639876", 4, 2001, 7, rate_root);
        insert_call(&head, "4342279876", 4, 1999, 7, rate_root);
        insert_call(&head, "43613529876", 4, 2010, 3, rate_root);
        insert_call(&head, "4369919339876", 4, 2001, 4, rate_root);
        insert_call(&head, "498282889876", 4, 1980, 7, rate_root);

        print_call_list(head, 0, 0);

        traverse_rates_postorder(rate_tree_root, delete_rate_node);
        rate_tree_root = NULL;
    #endif



    return EXIT_SUCCESS;
}