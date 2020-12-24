/**
 *      @file main.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The main procedure for the csv based phone billing project.
 * 
 *      https://github.com/NestorJankovic/c_phone_billing_system/tree/master
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

    getopt(argc, argv, "--");
    FILE *call_rates = open_csv(optarg);

    getopt(argc, argv, "--");
    FILE *call_record = open_csv(optarg);

    if ((call_rates == NULL) || (call_record == NULL)) {
        fprintf(stderr, "Error loading files, aborting\n");
        return EXIT_FAILURE;
    }
    

    close_csv(call_rates);
    close_csv(call_record);

    #ifdef DEBUG
    printf("%li, %li, %f\n", total_call_number, total_call_duration, total_call_price);
    #endif

    return EXIT_SUCCESS;
}