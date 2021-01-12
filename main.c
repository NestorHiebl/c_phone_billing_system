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

    user_node *user_root = parse_call_csv(call_record, rate_root);
    traverse_users_inorder(user_root, print_user_node);

    close_csv(call_rates);
    close_csv(call_record);

    // Calculate stats for all users
    traverse_users_preorder(user_root, calculate_user_stats);
    

    printf("%li, %li, %f\n", total_call_number, total_call_duration, total_call_price);

    #ifdef DEBUG

        char *censor_test = censor_calee_number("123456789");
    
        printf("Censor callee number test: %s\n", censor_test);
        free(censor_test); censor_test = NULL;

        user_node *test_user_node = user_root;

        generate_monthly_cdr_files(test_user_node);

    #endif

    traverse_rates_postorder(rate_root, delete_rate_node);
    rate_root = NULL;

    return EXIT_SUCCESS;
}