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
//#define DEBUG


size_t total_call_duration = 0;
size_t total_call_number = 0;
double total_call_price = 0;

int main(int argc, char **argv){

    if (argc < 2) {
        printf(     "Usage: main -r [Call rate CSV file] -c [Call record CSV file]\n"
                    "Generate monthly bill and CDR files for every calling party in the call "
                    "record file based on the call rate file. The rate filename has to be passed "
                    "with option -r and the call record filename has to be passed with option -c.\n"
                    "Optional arguments:\n"
                    "\t-h\tHelp\n");
            
            return EXIT_SUCCESS;
    }    


    char c = 0;

    FILE *call_rates = NULL;
    FILE *call_record = NULL;

    while ((c = getopt(argc, argv, "hr:c:")) != -1) {
        switch (c) {
        case 'h':
            printf( "Usage: main -r [Call rate CSV file] -c [Call record CSV file]\n"
                    "Generate monthly bill and CDR files for every calling party in the call "
                    "record file based on the call rate file. The rate filename has to be passed "
                    "with option -r and the call record filename has to be passed with option -c.\n"
                    "Optional arguments:\n"
                    "\t-h\tHelp\n");
            return EXIT_SUCCESS;
            break;
        case 'r':
            call_rates = open_csv(optarg);
            if (call_rates == NULL) {
                fprintf(stderr, "Could not open rate record \"%s\" - invalid filename\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'c':
            call_record = open_csv(optarg);
            if (call_record == NULL) {
                fprintf(stderr, "Could not open call record \"%s\" - invalid filename\n", optarg);
                return EXIT_FAILURE;
            }            
            break;
        default:
            fprintf(stderr, "Unknown option '%c' found\n", c);
            break;
        }
    }

    if ((call_rates == NULL) || (call_record == NULL)) {
        fprintf(stderr, "Error loading files, aborting execution\n");
        return EXIT_FAILURE;
    }

    printf("\nParsing rate record:\n");
    rate_node *rate_root = parse_rate_csv(call_rates);
    if (rate_root == NULL) {
        fprintf(stderr, "Error: No valid data was found in the rate record. Aborting execution\n");
        return EXIT_FAILURE;
    }

    #ifdef DEBUG
        printf("The rates found in their respetive file:\n");
        traverse_rates_inorder(rate_root, print_rate_node);
    #endif

    printf("\nParsing call record:\n");
    user_node *user_root = parse_call_csv(call_record, rate_root);
    if (user_root == NULL) {
        fprintf(stderr, "Error: No valid data was found in the call record. Aborting execution\n");
        return EXIT_FAILURE;
    }
    
    #ifdef DEBUG
        printf("The user profiles that were generated:\n");
        traverse_users_inorder(user_root, print_user_node);
    #endif

    close_csv(call_rates);
    close_csv(call_record);

    // Just to be safe
    traverse_users_preorder(user_root, calculate_user_stats);

    #ifdef DEBUG

        char *censor_test = censor_calee_number("123456789");
    
        printf("Censor callee number test: %s\n", censor_test);
        free(censor_test); censor_test = NULL;

        user_node *test_user_node = user_root;

        generate_monthly_bill_files(test_user_node);

    #endif

    printf("\nGenerating cdr files...\n");
    traverse_users_preorder(user_root, generate_monthly_cdr_files);
    printf("Generating bill files...\n\n");
    traverse_users_preorder(user_root, generate_monthly_bill_files);

    printf( "Total number of calls: %li\n"
            "Total duration of calls: %li (seconds)\n"
            "Total price of calls: %.2f €\n", total_call_number, total_call_duration, total_call_price);

    traverse_rates_postorder(rate_root, delete_rate_node);
    rate_root = NULL;
    traverse_users_postorder(user_root, delete_user_node);
    user_root = NULL;

    return EXIT_SUCCESS;
}