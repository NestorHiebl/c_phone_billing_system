/**
 *      @file csv_to_avl_tree.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external functions for the csv based phone billing project 
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
*/

#include <stdio.h>
#include <stdlib.h>
#include "csv_to_avl_tree.h"
#include <ctype.h>

#define CURRENT_YEAR 2021
#define TELEPHONE_INVENTION_YEAR 1876

/**
 *      Open CSV
 * 
 *      @brief Handles opening a csv file with a given name.
 * 
 *      @param filename The filename of the csv file.
 * 
 *      @returns The file pointer to given file, or NULL if the file is not found or handling has failed.
 */
FILE *open_csv(const char* filename){
    size_t filename_len = strlen(filename);
    
    // Checks if the passed filename ends in ".csv"
    if ((strcmp(&filename[filename_len - 4], ".csv")) != 0) {
        fprintf(stderr, "Please provide a valid csv file\n");
        return NULL;
    }

    FILE *csv = fopen(filename, "r");

    if (csv == NULL) {
        printf("Filename invalid, aborting\n");
        return NULL;
    }
    return csv;
}

/**
 *      Close CSV
 * 
 *      @brief Handles closing a csv file pointer.
 * 
 *      @param filename The pointer to csv file.
 * 
 *      @returns An integer indicator of the function's success
 *      @retval 0 If file closing failed
 *      @retval 1 If file closing was successful
 */
int close_csv(FILE *filepointer) {

    if (fflush(filepointer) != 0) {
        fprintf(stderr, "File flush failed\n");
        return 0;
    }

    if (fclose(filepointer) != 0) {
        fprintf(stderr, "File closing failed\n");
        return 0;
    }

    return 1;
}

/**
 *      Parse call csv
 * 
 *      The iterative logic for parsing rows and fields is based on @c fgets , @c strtok and @c sscanf , respectfully.
 *      Potential error scenarions are:
 *      @li A row in the csv is longer than 1024 characters. The function will notify you, but won't attempt to salvage the row.
 *      @li A field is missing. Strtok ignores consecutive delimiters, so any rows with NaN fields will be discarded.
 *      @li A datetime field is formatted incorrectly. The proper format is @c yyyy-mm-dd @c hh:mm:ss .
 * 
 *      @brief Builds a full user avl tree with a call linked list starting at each node list based on a csv file pointer.
 *      
 *      @param filename The @c FILE pointer for the csv.
 * 
 *      @returns A pointer to the root of the generated avl tree.
 */
user_node *parse_call_csv(FILE *filename, rate_node *rate_root) {

    char csv_line[1024];

    user_node *root = NULL;

    // Used for debugging
    size_t line_counter = 1;
    while (!(feof(filename))) {
        if ((fgets(csv_line, 1024, filename)) != NULL) {
            
            // Going smoothly, the line has been loaded in successfuly

            if (((csv_line[strlen(csv_line) - 1]) == '\n')) {

                // Remove the trailing newline from the csv row
                csv_line[strlen(csv_line) - 1] = '\0';
                
            } else if (feof(filename)) {
                // We've reached the end of the file
                // printf("Call file ended\n");    
            } else {
                // We're dealing with a really long line
                printf("Call line longer than 1024 characters\n");
                line_counter++;
                continue;
            } 
                
            char *caller_number_token = strtok(csv_line, ",");
            if (caller_number_token == NULL) {
                fprintf(stderr, "Call line %lu is empty\n", line_counter);
                line_counter++;
                continue;
            }            

            char *callee_number_token = strtok(NULL, ",");
            if (callee_number_token == NULL) {
                fprintf(stderr, "Call line %lu is missing three arguments\n", line_counter);
                line_counter++;
                continue;
            }
            
            char *duration_token = strtok(NULL, ",");
            if (duration_token == NULL) {
                fprintf(stderr, "Call line %lu is missing two arguments\n", line_counter);
                line_counter++;
                continue;
            }

            char *datetime_token = strtok(NULL, ",");
            if (datetime_token == NULL) {
                fprintf(stderr, "Call line %lu is missing one argument\n", line_counter);
                line_counter++;
                continue;
            }
            
            
            if (strtok(NULL, ",") != NULL) {
                fprintf(stderr, "Additional field found on call line %lu\n", line_counter);
                line_counter++;
                continue;
            }

            if (strcmp(caller_number_token, "Anonymous") == 0) {
                // Anonymous caller, increment total call counter, total call duration counter and continue
                // printf("Anonymous caller found on line %lu\n", line_counter);
                line_counter++;
                total_call_number++;
                total_call_duration += atoi(duration_token);
                // Not incrementing global price because anonymous callers cannot be billed
                continue;
            }
            

            caller_number_token = validate_phone_number(&caller_number_token);
            callee_number_token = validate_phone_number(&callee_number_token);
            size_t year_token = 0;
            size_t month_token = 0;
            size_t day_token = 0;

            // Date extraction happens here
            if ((sscanf(datetime_token, "%4lu-%2lu-%2lu %*d:%*d:%*d", &year_token, &month_token, &day_token)) != 3) {
                fprintf(stderr, "Error: Invalid date found on line %lu\n", line_counter);
                line_counter++;
                continue;
            } else if ((month_token > 12) || (year_token > CURRENT_YEAR) || (year_token < TELEPHONE_INVENTION_YEAR)) {
                fprintf(stderr, "Error: Invalid year/month found on line %lu\n", line_counter);
                line_counter++;
                continue;
            }

            // printf("Year: %lu, Month: %lu, Day: %lu\n", year_token, month_token, day_token);
            
            if ((caller_number_token != NULL) && (callee_number_token != NULL)) {
                
                /*********************************************************
                * The necesarry data has been collected, create the node *
                *********************************************************/
                
                root = add_user_node(root, caller_number_token, callee_number_token, atoi(duration_token), year_token, month_token, day_token, rate_root);

            } else {
                printf("Invalid caller or callee number found on call line %lu\n", line_counter);
                line_counter++;
                continue;
            }
            
        } else {
            // Couldn't load a line in
            fprintf(stderr, "Loading line %lu in csv call file failed, aborting\n", line_counter);
            return root;    
        }
        line_counter++;
    }
    return root;
}


/**
 *      Parse rate csv
 * 
 *      The iterative logic for parsing rows and fields is based on @c fgets and @c strtok , respectfully. Potential error scenarions are:
 *      @li A row in the csv is longer than 1024 characters. The function will notify you, but won't attempt to salvage the row.
 *      @li A field is missing. Strtok ignores consecutive delimiters, so any rows with NaN fields will be discarded.
 * 
 *      @brief Builds a full rate avl tree based on a csv file pointer.
 *      
 *      @param filename The @c FILE pointer for the csv.
 * 
 *      @returns A pointer to the root of the generated avl tree.
 */
rate_node *parse_rate_csv(FILE *filename) {

    char csv_line[1024];

    rate_node *root = NULL;

    // Used for debugging
    size_t line_counter = 1;
    while (!(feof(filename))) {
        if ((fgets(csv_line, 1024, filename)) != NULL) {
            
            // Going smoothly, the line has been loaded in successfuly

            if (((csv_line[strlen(csv_line) - 1]) == '\n')) {

                // Remove the trailing newline from the csv row
                csv_line[strlen(csv_line) - 1] = '\0';
                
            } else if (feof(filename)) {
                // We've reached the end of the file
                // printf("File ended\n");    
            } else {
                // We're dealing with a really long line
                printf("Line longer than 1024 characters\n");
                line_counter++;
                continue;
            } 
                
            char *region_code_token = strtok(csv_line, ",");
            if (region_code_token == NULL) {
                fprintf(stderr, "Line %lu is empty\n", line_counter);
                line_counter++;
                continue;
            }
            
            char *region_token = strtok(NULL, ",");
            if (region_token == NULL) {
                fprintf(stderr, "Line %lu is missing two arguments\n", line_counter);
                line_counter++;
                continue;
            }

            char *rate_token = strtok(NULL, ",");
            if (rate_token == NULL) {
                fprintf(stderr, "Line %lu is missing one argument\n", line_counter);
                line_counter++;
                continue;
            }

            rate_token = validate_rate(rate_token);
            if (rate_token == NULL) {
                fprintf(stderr, "Invalid rate found on line %lu\n", line_counter);
                line_counter++;
                continue;
            }
            
            double rate_token_d = strtod(rate_token, NULL);
            
            if (strtok(NULL, ",") != NULL) {
                fprintf(stderr, "Additional field found on line %lu\n", line_counter);
                line_counter++;
                continue;
            }

            region_code_token = validate_region_code(&region_code_token);
            
            if (region_code_token != NULL) {
                
                /*********************************************************
                * The necesarry data has been collected, create the node *
                *********************************************************/
                
                root = add_rate_node(root, region_code_token, rate_token_d);

            } else {
                printf("Invalid region code found on line %lu\n", line_counter);
                line_counter++;
                continue;
            }
            
        } else {
            // Couldn't load a line in
            fprintf(stderr, "Loading line %lu in csv file failed, aborting\n", line_counter);
            return root;    
        }
        line_counter++;
    }
    return root;
}


/**
 *      Generate cdr filename
 *      @brief Generates a filename for a user's monthly cdr record based on the user's number and the year and month.
 *      Note that the returned filename is in a memory block that needs to be freed manually.
 *      
 *      @param user_number The user's number in @c string format.
 *      @param datetime The year of the record multiplied by 100, plus the month of the record.
 *      @return The filename or @c NULL if the function failed.
 */
char *generate_cdr_filename(char *user_number, size_t datetime) {
    size_t month = datetime % 100;
    size_t year = (datetime - month) / 100;

    if ((month > 12) || (year < TELEPHONE_INVENTION_YEAR) || (year > CURRENT_YEAR)) {
        fprintf(stderr, "Invalid date found in cdr filename generator\n");
        return NULL;
    }    

    char *cdr_filename = malloc(strlen(user_number) + 20 /* 14 bytes are necessarry, 20 for extra breathing room*/);
    if (cdr_filename == NULL) {
        fprintf(stderr, "Failed to allocate memory for cdr filename\n");
        return NULL;
    }
    
    sprintf(cdr_filename, "%s-%lu-%lu-cdr.txt", user_number, month, year);

    return cdr_filename;
}

/**
 *      Generate monthly bill filename
 *      @brief Generates a filename for a user's monthly bill based on the user's number and the year and month.
 *      Note that the returned filename is in a memory block that needs to be freed manually.
 *      
 *      @param user_number The user's number in @c string format.
 *      @param datetime The year of the record multiplied by 100, plus the month of the record.
 *      @return The filename or @c NULL if the function failed.
 */
char *generate_monthly_bill_filename(char *user_number, size_t datetime) {
    size_t month = datetime % 100;
    size_t year = (datetime - month) / 100;

    if ((month > 12) || (year < TELEPHONE_INVENTION_YEAR) || (year > CURRENT_YEAR)) {
        fprintf(stderr, "Invalid date found in cdr filename generator\n");
        return NULL;
    }

    char *monthly_bill_filename = malloc(strlen(user_number) + 20 /* 9 bytes are necessarry, 20 for extra breathing room*/);
    if (monthly_bill_filename == NULL) {
        fprintf(stderr, "Failed to allocate memory for montly bill filename\n");
        return NULL;
    }

    sprintf(monthly_bill_filename, "%s-%lu-%lu.txt", user_number, month, year);

    return monthly_bill_filename;
}

/**
 *      Open monthly cdr bill
 *      @brief Opens a cdr file in write mode based on a filename.
 *      
 *      @param filename The filename of the cdr record to be opened.
 *      @return A file pointer to the file if successfull, otherwise @c NULL .
 */
FILE *open_monthly_cdr_bill(char *filename) {
    FILE *cdr_bill = fopen(filename, "w");
    if (cdr_bill == NULL) {
        fprintf(stderr, "Could not open file \"%s\", aborting\n", filename);
        return NULL;
    }
    return cdr_bill;    
}

/**
 *      Close monthly cdr bill
 *      @brief Close a file pointer to a cdr record file.
 *      
 *      @param filepointer The file to be closed.
 *      @return 1 if successfull, 0 if not.
 */
int close_monthly_cdr_bill(FILE *filepointer) {
    if (filepointer == NULL) {
        fprintf(stderr, "Cannot close NULL pointer\n");
        return 0;
    }
    
    if (fflush(filepointer) != 0) {
        fprintf(stderr, "Flushing cdr file failed, aborting\n");
        return 0;
    }

    if (fclose(filepointer) != 0) {
        fprintf(stderr, "Closing cdr file failed, aborting\n");
        return 0;
    }
    return 1;
}

/*****************************************************************************************************************
 * PATTERN CHECKING FUNCTIONS                                                                                    *
 *****************************************************************************************************************/

/**
 *      Validate phone number
 *      @brief Checks if a phone number is legal based on E.164 and removes leading zeros.
 *      
 *      @param phone_number The number to be checked.
 *      @return A pointer to the validated number or NULL if it was found to be invalid.
 */
char *validate_phone_number(char **phone_number) {
    if (*phone_number == NULL) {
        fprintf(stderr, "Cannot validate NULL string\n");
        return NULL;
    }
    
    while (**phone_number == '0') {
        *phone_number = (*phone_number) + 1;
    }

    size_t phone_number_len = strlen(*phone_number);

    if (phone_number_len > 15 /* Should be correct according to E.164 */) {
        fprintf(stderr, "Phone number too long, aborting\n");
        return NULL;
    }

    // Initialize the legality flag as zero to exclude region_codes that don't enter the loop at all (len = 0)
    _Bool legal = 0;

    for (size_t i = 0; i < phone_number_len; i++) {
        legal = 1;

        if (!(isdigit((*phone_number)[i]))) {
            legal = 0;
            break;
        }
    }

    return legal ? *phone_number : NULL;    
}

/**
 *      Censor callee number
 *      @brief Replaces the final three digits of a phone number with '*'.
 *      Note that the censored number is in a new memory block that needs to be freed.
 *      
 *      @param callee_number The number to be cenosored
 *      @return A pointer to the censored number or @c NULL if the function failed.
 */
char *censor_calee_number(const char *callee_number) {
    size_t callee_number_len = strlen(callee_number);

    if (callee_number_len < 3) {
        fprintf(stderr, "Callee number \"%s\" too short to be censored\n", callee_number);
        return NULL;
    }

    char *callee_number_censored = malloc(callee_number_len + 1);
    if (callee_number_censored == NULL) {
        fprintf(stderr, "Couldn't allocate memory for censoring a phone number\n");
        return NULL;
    }
    
    strcpy(callee_number_censored, callee_number);

    for (size_t i = callee_number_len - 3; i < callee_number_len; i++) {
        *(callee_number_censored + i) = '*';
    }
    
    return callee_number_censored;
}

/**
 *      Validate region_code
 * 
 *      @brief Checks if a given region code is legal in E.164 and formats it by removing leading zeros, if need be.
 * 
 *      @param region_code A double pointer to the region code. May be altered.
 *  
 *      @return A pointer to the valid region code or NULL
 */
char *validate_region_code(char **region_code){
    if (*region_code == NULL) {
        fprintf(stderr, "Cannot validate NULL string\n");
        return NULL;
    }
    
    // Remove leading zeros
    while ((**region_code == '0') || (**region_code == '+')) {
        *region_code = (*region_code) + 1;
    }
    
    size_t region_code_len = strlen(*region_code);

    if (region_code_len > 11) {
        // region_code too long, invalid
        return NULL;
    }
    
    // Initialize the legality flag as zero to exclude region_codes that don't enter the loop at all (len = 0)
    _Bool legal = 0;

    for (size_t i = 0; i < region_code_len; i++) {
        legal = 1;

        if (!(isdigit((*region_code)[i]))) {
            legal = 0;
            break;
        }
    }

    return legal ? *region_code : NULL;
}

/**
 *      Validate rate
 *      @brief Checks if a string contains a valid call rate. Note that a string of dots will confuse the algorithm
 *      
 *      @param rate The rate to be checked.
 *      @returns The unchanged rate string if valid, otherwise NULL.
 */
char *validate_rate(char *rate){
    if (rate == NULL) return NULL;

    size_t i = 0;
    while (*(rate + i) != '\0') {
        if (!(isdigit(*(rate + i))) && (*(rate + i) != '.')) {
            return NULL;
        }
        i++;
    }
    return rate;    
}

/**
 *      Search by longest region code match
 *      @brief Finds the longest region code match of a number in a rate binary search tree.
 *      
 *      @param root The root of the rate tree to be searched for.
 *      @param callee_number The number whose longest region code match is to be found.
 *      @return The rate node with the longest match, or @c NULL if no matches have been found.
 * 
 */
rate_node *search_by_longest_region_code_match(rate_node *root, const char *callee_number) {
    if (callee_number == NULL) {
        fprintf(stderr, "Cannot search for NULL region code, aborting\n");
        return NULL;
    }
    
    rate_node *current_longest_match = NULL;

    size_t attempt_length = 1;

    while (attempt_length <= strlen(callee_number)) {
        //The block below copies the n starting digits of the callee number inro an array, then attempts to find a match
        char callee_number_leading_segement[attempt_length + 1];        
        strncpy(callee_number_leading_segement, callee_number, attempt_length);
        callee_number_leading_segement[attempt_length] = '\0';

        // Only save the search results if they are not NULL
        rate_node *new_attempt = search_rate_tree(root, callee_number_leading_segement);
        if (new_attempt != NULL) {
            current_longest_match = new_attempt;
        }
        attempt_length++;
    }
    //No match has been found
    if (current_longest_match == NULL) printf("No match found for callee number \"%s\", aborting search\n", callee_number);
    
    return current_longest_match;
}

/**
 *      Calculate call seconds
 *      @brief Calculate the second component of a call's duration.
 *      
 *      @param duration The duration of a call.
 *      @return The second component of a call.
 */
size_t calculate_call_seconds(size_t duration) {
    return duration % 60;
}

/**
 *      Calculate call minutes
 *      @brief Calculate the minute component of a call's duration.
 *      
 *      @param duration The duration of a call.
 *      @return The minute component of a call.
 */
size_t calculate_call_minutes(size_t duration) {
    return (duration - (3600 * (duration / 3600))) / 60;
}

/**
 *      Calculate call hours
 *      @brief Calculate the hour component of a call's duration.
 *      
 *      @param duration The duration of a call.
 *      @return The hour component of a call.
 */
size_t calculate_call_hours(size_t duration) {
    return duration / 3600;
}

/**
 *      Max
 *      @brief Checks which of two integers is greater.
 *      
 *      @param a The first int to be compared.
 *      @param b The second int to be compared.
 *      @returns @c a if @c a is greater, @c b if @c b is equal or greater.
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/*****************************************************************************************************************
 * CALL LINKED LIST FUNCTIONS                                                                                    *
 *****************************************************************************************************************/

/**
 *      Insert call node
 * 
 *      @brief Inserts a new node into the call linked list so that the it remains ordered.
 *      If the head argument is NULL, it initializes a list instead.
 *      
 *      @param head A double pointer to the head of the list, which will be changed dynamically.
 *      @param tail A double pointer to the tail of the list, which will be changed dynamically.
 *      @param callee_number The number that was called, cannot be NULL.
 *      @param duration The duration of the call.
 *      @param year The year the call took place in.
 *      @param month The month the call took place in.
 *      @param rate_root The root of the rate AVL tree in which the relevant region codes are stored.
 * 
 *      @returns 1 if the function suceeds, 0 if it fails.
 */
int insert_call(user_call_list **head, char *callee_number, size_t duration, size_t year, size_t month, size_t day, rate_node *rate_root) {

    if (callee_number == NULL) {
        fprintf(stderr, "Callee number string empty, aborting\n");
        return 0;
    }
    
    user_call_list *new_node = malloc(sizeof(user_call_list));
    if (new_node == NULL) {
        fprintf(stderr, "Not enough memory to create new call linked list node\n");
        return 0;
    }

    // Initialize callee number
    new_node->callee = malloc(strlen(callee_number) + 1);
    if (new_node->callee == NULL) {
        fprintf(stderr, "Not enough memory to initialize callee number\n");
        return 0;
    } else {
        strcpy(new_node->callee, callee_number);    
    }
    
    new_node->duration = duration;
    new_node->year = year;
    new_node->month = month;
    new_node->day = day;

    rate_node *longest_rate_match = search_by_longest_region_code_match(rate_root, callee_number);

    if (longest_rate_match == NULL) {
        fprintf(stderr, "No rate match found for the number \"%s\", call price set to zero\n", new_node->callee);
        new_node->price = 0;
    } else {
        new_node->price = (double) longest_rate_match->rate * duration;
    }

    // Global counters incremented here
    total_call_number++;
    total_call_duration += new_node->duration;
    total_call_price += new_node->price;
    
    if ((*head == NULL)) {
        // The list is being initialized

        new_node->previous = NULL;
        new_node->next = NULL;

        *head = new_node;
    } else {
        // The node is being appended to an existing list
        
        if (get_call_node_datetime(new_node) <= get_call_node_datetime(*head)) {
            // The new node comes before root
            new_node->next = *head;
            new_node->previous = NULL;
            (*head)->previous = new_node;
            *head = new_node;

            return 1;
        } else {
            user_call_list *current = *head;

            while (current != NULL) {
                
                if (get_call_node_datetime(current) > get_call_node_datetime(new_node)) {
                    // We found the node directly after the new node
                    current->previous->next = new_node;
                    new_node->previous = current->previous;
                    new_node->next = current;
                    current->previous = new_node;                   

                    return 1;
                } else if (current->next == NULL) {
                    // Appending to end of list

                    current->next = new_node;
                    new_node->previous = current;
                    new_node->next = NULL;

                    return 1;
                }
                current = current->next;
            }
        }
    }
    // Shouldn't happen
    return 0;
}

/**
 *      Get call node datetime
 *      @brief Get the call node datetime.
 *      
 *      @param node The call whose datetime is to be returned.
 *      @return The year of the call multiplied by 100 plus the month of the call.
 */
size_t get_call_node_datetime(user_call_list *node) {
    return (node == NULL) ? 0 : (node->year * 100) + node->month;
}

/**
 *      Print user call list
 * 
 *      @brief Prints a slice of a call linked list. If both indexes are set to 0, prints the entire list instead.
 * 
 *      @param head The head of the linked list.
 *      @param start_index The starting index of the list slice.
 *      @param end_index The ending index of the list slice.
 */
void print_call_list(user_call_list *head, size_t start_index, size_t end_index) {
    if (head == NULL) {
        fprintf(stderr, "Cannot print NULL list, aborting\n");
        return;
    } else if (start_index > end_index) {
        fprintf(stderr, "Starting index larger than ending index, aborting");
        return;
    }
    
    
    user_call_list *current = head;
    if ((start_index == 0) && (end_index == 0)) {
        // If no start and end indexes were given print the whole list

        while (current != NULL) {
        printf( "The called number is: \"%s\", "
                "The price of the call is: %.2f, "
                "and it took place in month %lu of %lu.\n", current->callee, current->price, current->month, current->year);
        current = current->next;
        }    
    } else {
        // Print list from start index to end index
        size_t i = 0;

        while (current != NULL) {
            if (i >= start_index) {
                // If we've reached the start index, print the node
                printf( "The called number is: \"%s\","
                        "The price of the call is: %.2f,\n"
                        "and it took place in month %lu of %lu.\n", current->callee, current->price, current->month, current->year);
            }

            if (i > end_index) {
                // If we've exceeded the end index, exit the while loop
                break;
            }
            i++;
            current = current->next;
        }
    }
    return;   
}

/**
 *      Delete call linked list     
 * 
 *      @brief Deletes a call linked list, freeing the memory it took up.
 * 
 *      @param head A double pointer to the head of the list. Will be set to NULL.
 *      @return 1 if successful, 0 if not. 
 */
int delete_call_list(user_call_list **head) {
    if (*head == NULL) {
        fprintf(stderr, "Cannot delete NULL list, aborting\n");
        return 0;
    }

    user_call_list *current = *head;

    while (*head != NULL) {
        current = *head;

        free(current->callee);                       
        current->callee = NULL;

        *head = (*head)->next;
        free(current);
        current = NULL;
    }
    
    return 1;
}

/*****************************************************************************************************************
 * AVL RATE TREE FUNCTIONS                                                                                       *
 ****************************************************************************************************************/

/**
 *      Insert rate tree node
 * 
 *      @brief Recursively inserts a new rate node to the rate AVL tree. The tree is automatically rebalanced in the process.
 *      Interfacing with the rate AVL tree should only be done through this function and the traversals.
 *      
 *      @param node A pointer to the tree root. May change due to rebalancing.
 *      @param region_code The region_code string, cannot be NULL.
 *      @param rate The rate associated with the region_code.
 * 
 *      @returns The tree's new root.
 */
rate_node *add_rate_node(rate_node *node, const char *region_code, double rate) {
    if (region_code == NULL) {
        fprintf(stderr, "Number string empty, aborting\n");
        return NULL;
    }

    if (node == NULL){
        return make_rate_node(region_code, rate);
    }

    if (strcmp(region_code, node->region_code) < 0) {
        // Going left
        node->left = add_rate_node(node->left, region_code, rate);
    } else if (strcmp(region_code, node->region_code) > 0) {
        // Going right
        node->right = add_rate_node(node->right, region_code, rate);
    } else {
        // This should not happen
        fprintf(stderr, "Error: region code \"%s\" already found in tree\n", region_code);
        return node;
    }
    
    // Update node height
    node->height = 1 + max(get_rate_node_height(node->left), get_rate_node_height(node->right));

    // Get node balance    
    int balance = get_rate_node_balance(node);

    // Imbalance is in left child's left subtree
    if ((balance > 1) && (strcmp(region_code, node->left->region_code) < 0)) {
        return right_rotate_rate(node);
    }

    // Imbalance is in right child's right subtree
    if ((balance < -1) && (strcmp(region_code, node->right->region_code) > 0)) {
        return left_rotate_rate(node);
    }
    
    // Imbalance is in left child's right subtree
    if ((balance > 1) && (strcmp(region_code, node->left->region_code) > 0)) {
        node->left = left_rotate_rate(node->left);
        return right_rotate_rate(node);
    }

    // Imbalance is in right child's left subtree
    if ((balance < -1) && (strcmp(region_code, node->right->region_code) < 0)) {
        node->right = right_rotate_rate(node->right);
        return left_rotate_rate(node);
    }

    // Return the current node, unchanged
    return node;    
}

/**
 *      Make rate node
 *      @brief Initializes a new rate node and returns a pointer to it. Is called internally by @c add_rate_node.
 *      
 *      @param region_code The region_code string, cannot be NULL.
 *      @param rate The rate associated with the region_code.
 *      @returns A pointer to the new rate node, or NULL if there was an error.
 */
rate_node *make_rate_node(const char *region_code, double rate) {
    if (region_code == NULL) {
        fprintf(stderr, "Number string empty, aborting\n");
        return NULL;
    }

    rate_node *newNode = malloc(sizeof(rate_node));
    if (newNode == NULL) {
        fprintf(stderr, "Not enough memory to create new rate node, aborting\n");
        return NULL;
    }
    
    newNode->region_code = malloc(strlen(region_code) + 1);
    if (newNode->region_code == NULL) {
        fprintf(stderr, "Not enough memory to initialize region code field, aborting\n");
        free(newNode);
        newNode = NULL;
        return NULL;
    } else {
        strcpy(newNode->region_code, region_code);
    }
    
    newNode->rate = rate;

    newNode->height = 1;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

/**
 *      Right rotate rate node
 *      @brief Performs a right rotation around a given node's left child. Should only be called by the @c add_rate_node function.
 *      
 *      @param node The node to be right rotated
 *      @returns The passed node's left child, which is the new subtree root.
 */
rate_node *right_rotate_rate(rate_node *node) {
    rate_node *leftChild = node->left;
    rate_node * leftChildRight = leftChild->right;

    // Rotate around left child
    leftChild->right = node;
    node->left = leftChildRight;

    // Update heights
    node->height = 1 + max(get_rate_node_height(node->left), get_rate_node_height(node->right));
    leftChild->height = 1 + max(get_rate_node_height(leftChild->left), get_rate_node_height(leftChild->right));

    // The left child is now the subtree root
    return leftChild;
}

/**
 *      Left rotate rate node
 *      @brief Performs a left rotation around a given node's right child. Should only be called by the @c add_rate_node function.
 *      
 *      @param node The node to be left rotated
 *      @returns The passed node's right child, which is the new subtree root.
 */
rate_node *left_rotate_rate(rate_node *node) {
        rate_node *rightChild = node->right;
        rate_node * rightChildLeft = rightChild->left;

        // Rotate around right child
        rightChild->left = node;
        node->right = rightChildLeft;

        // Update heights
        node->height = 1 + max(get_rate_node_height(node->left), get_rate_node_height(node->right));
        rightChild->height = 1 + max(get_rate_node_height(rightChild->left), get_rate_node_height(rightChild->right));

        // The right child is now the subtree root
        return rightChild;
}

/**
 *      Traverse rates inorder
 *      @brief Traverses a given rate tree inorder. Useful for printing.
 *      
 *      @param node The root of the tree
 *      @param visit The function to be performed upon visiting
 */
void traverse_rates_inorder(rate_node *node, void (*visit) (rate_node*)) {
    if (node == NULL) return;

    traverse_rates_inorder(node->left, visit);
    visit(node);
    traverse_rates_inorder(node->right, visit);
}

/**
 *      Traverse rates postorder
 *      @brief Traverses a given rate tree postorder. Useful for deletition.
 *      
 *      @param node The root of the tree
 *      @param visit The function to be performed upon visiting
 */
void traverse_rates_postorder(rate_node *node, void (*visit) (rate_node*)) {
    if (node == NULL) return;

    traverse_rates_postorder(node->left, visit);
    traverse_rates_postorder(node->right, visit);
    visit(node);
}

/**
 *      Print rate node
 *      @brief Prints a single node in a rate AVL tree. Can be used for traversals.
 *      
 *      @param node The node to be printed
 */
void print_rate_node(rate_node *node) {
    if (node == NULL) {
        printf("Node is NULL\n");
        return;
    }
    printf("Key: %s, Rate: %f\n", node->region_code, node->rate);
    return;
}

/**
 *      Delete rate node
 *      @brief Deletes a single node from a rate AVL tree. Can be used for (postorder) traversals.
 *      
 *      @param node The node to be deleted
 */
void delete_rate_node(rate_node *node) {
    if (node == NULL) {
        fprintf(stderr, "Cannot delete NULL node\n");
        return;
    }
    free(node->region_code);
    node->region_code = NULL;
    node->left = NULL;
    node->right = NULL;

    free(node);
}

/**
 *      Get rate node height
 *      @brief Get the rate node height
 *      
 *      @param node The node whose height is to be checked
 *      @returns @c 0 if the node is @c NULL, otherwise its height
 */
int get_rate_node_height(rate_node *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;    
}

/**
 *      Get rate node balance
 *      @brief Get the rate node balance
 *      
 *      @param node The node whose balance is to be checked
 *      @returns @c 0 if the node is @c NULL, otherwise its balance
 */
int get_rate_node_balance(rate_node *node) {
    if (node == NULL) {
        return 0;
    }
    return get_rate_node_height(node->left) - get_rate_node_height(node->right);    
}

/**
 *      Search rate tree
 *      @brief Recursively search through rate tree, based on a given region code.
 *      
 *      @param root The root of the tree to be searched.
 *      @param region_code The region code to search for. Cannot be @c NULL .
 *      @return The rate node with the appropriate region code, or @c NULL if no node was found.
 */
rate_node *search_rate_tree(rate_node *root, const char *region_code) {
    if (region_code == NULL) {
        fprintf(stderr, "Cannot search for NULL string, aborting\n");
        return NULL;
    }
    
    if (root == NULL) {
        return NULL;
    }
    

    if ((strcmp(region_code, root->region_code) == 0)) {
        return root;
    }
    
    if (strcmp(region_code, root->region_code) < 0) {
        return search_rate_tree(root->left, region_code);
    } else {
        return search_rate_tree(root->right, region_code);
    }
    
    return NULL;
}

/*****************************************************************************************************************
 * AVL USER TREE FUNCTIONS                                                                                       *
 ****************************************************************************************************************/

/**
 *      Insert user tree node
 * 
 *      @brief Recursively inserts a new user node to the user AVL tree. The tree is automatically rebalanced in the process.
 *      Interfacing with the user AVL tree should only be done through this function and the traversals.
 *      
 *      @param node A pointer to the tree root. May change due to rebalancing.
 *      @param caller_number The user number string, cannot be NULL.
 *      @param callee_number The number being called, cannot be NULL.
 *      @param duration The duration of the call.
 *      @param year The year the call took place in.
 *      @param month The month the call took place in.
 *      @param rate_root The root of the rate tree that the rate plans are stored in.
 * 
 *      @returns The tree's new root.
 */
user_node *add_user_node(user_node *node, const char *caller_number, char *callee_number, size_t duration, size_t year, size_t month, size_t day, rate_node *rate_root) {
    if (caller_number == NULL) {
        fprintf(stderr, "Number string empty, aborting\n");
        return NULL;
    }

    if (node == NULL){

        user_node *temp_new_user_node = make_user_node(caller_number);

        // Inserting into the call linked list
        insert_call(&(temp_new_user_node->call_list_head), callee_number, duration, year, month, day, rate_root);

        calculate_user_stats(temp_new_user_node);

        return temp_new_user_node;
    }

    if (strcmp(caller_number, node->number) < 0) {
        // Going left
        node->left = add_user_node(node->left, caller_number, callee_number, duration, year, month, day, rate_root);
    } else if (strcmp(caller_number, node->number) > 0) {
        // Going right
        node->right = add_user_node(node->right, caller_number, callee_number, duration, year, month, day, rate_root);
    } else {
        // The user already has a node - in this case we just want to add to their call data linked list
        // printf("User present in tree, appending call data\n");

        // Inserting into the call linked list
        insert_call(&(node->call_list_head), callee_number, duration, year, month, day, rate_root);

        calculate_user_stats(node);

        return node;
    }
    
    // Update node height
    node->height = 1 + max(get_user_node_height(node->left), get_user_node_height(node->right));

    // Get node balance    
    int balance = get_user_node_balance(node);

    // Imbalance is in left child's left subtree
    if ((balance > 1) && (strcmp(caller_number, node->left->number) < 0)) {
        return right_rotate_user(node);
    }

    // Imbalance is in right child's right subtree
    if ((balance < -1) && (strcmp(caller_number, node->right->number) > 0)) {
        return left_rotate_user(node);
    }
    
    // Imbalance is in left child's right subtree
    if ((balance > 1) && (strcmp(caller_number, node->left->number) > 0)) {
        node->left = left_rotate_user(node->left);
        return right_rotate_user(node);
    }

    // Imbalance is in right child's left subtree
    if ((balance < -1) && (strcmp(caller_number, node->right->number) < 0)) {
        node->right = right_rotate_user(node->right);
        return left_rotate_user(node);
    }

    // Return the current node, unchanged
    return node;    
}

/**
 *      Make user node
 *      @brief Initializes a new user node and returns a pointer to it. Is called internally by @c add_user_node.
 *      
 *      @param caller_number The user's number string, cannot be NULL.
 *      @returns A pointer to the new user node, or NULL if there was an error.
 */
user_node *make_user_node(const char *caller_number) {
    if (caller_number == NULL) {
        fprintf(stderr, "Caller number string empty, aborting\n");
        return NULL;
    }

    user_node *newNode = malloc(sizeof(user_node));
    if (newNode == NULL) {
        fprintf(stderr, "Not enough memory to create new user node, aborting\n");
        return NULL;
    }
    
    newNode->number = malloc(strlen(caller_number) + 1);
    if (newNode->number == NULL) {
        fprintf(stderr, "Not enough memory to initialize caller number field, aborting\n");
        free(newNode);
        newNode = NULL;
        return NULL;
    } else {
        strcpy(newNode->number, caller_number);
    }
    
    newNode->total_bill = 0;
    newNode->total_call_duration = 0;
    newNode->total_call_number = 0;

    newNode->call_list_head = NULL;

    newNode->height = 1;

    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

/**
 *      Right rotate user node
 *      @brief Performs a right rotation around a given node's left child. Should only be called by the @c add_user_node function.
 *      
 *      @param node The node to be right rotated
 *      @returns The passed node's left child, which is the new subtree root.
 */
user_node *right_rotate_user(user_node *node) {
    user_node *leftChild = node->left;
    user_node * leftChildRight = leftChild->right;

    // Rotate around left child
    leftChild->right = node;
    node->left = leftChildRight;

    // Update heights
    node->height = 1 + max(get_user_node_height(node->left), get_user_node_height(node->right));
    leftChild->height = 1 + max(get_user_node_height(leftChild->left), get_user_node_height(leftChild->right));

    // The left child is now the subtree root
    return leftChild;
}

/**
 *      Left rotate user node
 *      @brief Performs a left rotation around a given node's right child. Should only be called by the @c add_user_node function.
 *      
 *      @param node The node to be left rotated
 *      @returns The passed node's right child, which is the new subtree root.
 */
user_node *left_rotate_user(user_node *node) {
        user_node *rightChild = node->right;
        user_node * rightChildLeft = rightChild->left;

        // Rotate around right child
        rightChild->left = node;
        node->right = rightChildLeft;

        // Update heights
        node->height = 1 + max(get_user_node_height(node->left), get_user_node_height(node->right));
        rightChild->height = 1 + max(get_user_node_height(rightChild->left), get_user_node_height(rightChild->right));

        // The right child is now the subtree root
        return rightChild;
}

/**
 *      Traverse users preorder
 *      @brief Traverses a given user tree preorder. Useful for generating the bill files.
 *      
 *      @param node The root of the tree.
 *      @param visit The function to be performed upon visiting.
 */
void traverse_users_preorder(user_node *node, void (*visit) (user_node*)) {
    if (node == NULL) return;

    visit(node);
    traverse_users_preorder(node->left, visit);
    traverse_users_preorder(node->right, visit);
}

/**
 *      Traverse users inorder
 *      @brief Traverses a given user tree inorder. Useful for printing.
 *      
 *      @param node The root of the tree.
 *      @param visit The function to be performed upon visiting.
 */
void traverse_users_inorder(user_node *node, void (*visit) (user_node*)) {
    if (node == NULL) return;

    traverse_users_inorder(node->left, visit);
    visit(node);
    traverse_users_inorder(node->right, visit);
}

/**
 *      Traverse users postorder
 *      @brief Traverses a given user tree postorder. Useful for deletition.
 *      
 *      @param node The root of the tree
 *      @param visit The function to be performed upon visiting
 */
void traverse_users_postorder(user_node *node, void (*visit) (user_node*)) {
    if (node == NULL) return;

    traverse_users_postorder(node->left, visit);
    traverse_users_postorder(node->right, visit);
    visit(node);
}

/**
 *      Print user node
 *      @brief Prints a single node in a user AVL tree. Can be used for traversals.
 *      
 *      @param node The node to be printed
 */
void print_user_node(user_node *node) {
    if (node == NULL) {
        printf("Node is NULL\n");
        return;
    }
    printf( "Number: %s, "
            "Total call price: %f, "
            "Total call amount: %lu, "
            "Total call duration: %lu\n", node->number, node->total_bill, node->total_call_number, node->total_call_duration);
    return;
}

/**
 *      Delete user node
 *      @brief Deletes a single node from a user AVL tree. Can be used for (postorder) traversals.
 *      Note that there is an emberred linked list deletition procedure in this function.
 *      
 *      @param node The node to be deleted
 */
void delete_user_node(user_node *node) {
    if (node == NULL) {
        fprintf(stderr, "Cannot delete NULL node\n");
        return;
    }
    free(node->number);
    node->number = NULL;

    delete_call_list(&(node->call_list_head));

    node->left = NULL;
    node->right = NULL;

    free(node);
}

/**
 *      Get user node height
 *      @brief Get the user node height
 *      
 *      @param node The node whose height is to be checked
 *      @returns @c 0 if the node is @c NULL, otherwise its height
 */
int get_user_node_height(user_node *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;    
}

/**
 *      Get user node balance
 *      @brief Get the user node balance
 *      
 *      @param node The node whose balance is to be checked
 *      @returns @c 0 if the node is @c NULL, otherwise its balance
 */
int get_user_node_balance(user_node *node) {
    if (node == NULL) {
        return 0;
    }
    return get_user_node_height(node->left) - get_user_node_height(node->right);    
}

/**
 *      Calculate user stats
 *      @brief Calculates the total bill, call duration and call number for a given user.
 *      The appropriate parameters of the user node are automatically updated.
 *      
 *      @param user The user whose stats are to be calculated.
 */
void calculate_user_stats(user_node *user) {
    if (user == NULL) {
        fprintf(stderr, "Cannot calculate stats for NULL user, aborting\n");
        return;
    }

    user->total_bill = 0;
    user->total_call_duration = 0;
    user->total_call_number = 0;

    user_call_list *user_call_list_current = user->call_list_head;

    while (user_call_list_current != NULL) {
        user->total_bill += user_call_list_current->price;
        user->total_call_duration += user_call_list_current->duration;
        user->total_call_number++;

        user_call_list_current = user_call_list_current->next;
    }
    return;
}

/**
 *      Generate monthly cdr files
 *      @brief Generate the cdr record files for a given user. The user's call record linked list is iterated through for data.
 *      
 *      @param user The user for whom records are to be generated.
 */
void generate_monthly_cdr_files(user_node *user) {
    // The current call being processed
    user_call_list *current_user_call = user->call_list_head;

    // The file the call is to be logged in
    FILE *current_monthly_cdr_bill = NULL;

    // Iterate through the call list, is jumped over if the call list is NULL
    while (current_user_call != NULL) {
        // Get new datetime every time a month has been concluded
        size_t current_datetime = get_call_node_datetime(current_user_call);

        // If the file from the previous month is still open, close it
        if (current_monthly_cdr_bill != NULL) {
            close_monthly_cdr_bill(current_monthly_cdr_bill);
            current_monthly_cdr_bill = NULL;
        }
        
        // Generate the filename for the current month
        char *filename = generate_cdr_filename(user->number, current_datetime);

        // Create a file for the current month
        current_monthly_cdr_bill = open_monthly_cdr_bill(filename);
        if (current_monthly_cdr_bill == NULL) {
            fprintf(stderr, "Opening file \"%s\" has failed, aborting program\n", filename);
            exit(1);
        }
        
        // Keep writing to the same file until the call month changes
        while (current_datetime == get_call_node_datetime(current_user_call)) {
            // Censor callee number
            char *callee_number_censored = censor_calee_number(current_user_call->callee);

            // Calculate call timecode
            size_t call_seconds = calculate_call_seconds(current_user_call->duration);
            size_t call_minutes = calculate_call_minutes(current_user_call->duration);
            size_t call_hours = calculate_call_hours(current_user_call->duration);

            // Print to the file
            fprintf(current_monthly_cdr_bill, "%s, %s, %ld:%02ld:%02ld, %ld-%ld-%ld\n", 
                        user->number, 
                        callee_number_censored, 
                        call_hours, 
                        call_minutes, 
                        call_seconds,
                        current_user_call->year,
                        current_user_call->month,
                        current_user_call->day);

            // None of these memory blocks are needed at this point
            free(filename);
            filename = NULL;
            free(callee_number_censored);
            callee_number_censored = NULL;
            
            // If there is no next call, finish up file handling and exit the function.
            if(current_user_call->next == NULL) {
                if (current_monthly_cdr_bill != NULL) {
                    close_monthly_cdr_bill(current_monthly_cdr_bill);
                }

                // Break to avoid segfault
                return;
            }
            // Go to the next call
            current_user_call = current_user_call->next;
        }
    }
    fprintf(stderr, "Cannot generate monthly bills for user with no calls\n");
    return;
}

void generate_monthly_bill_files(user_node *user) {
    // The current call being processed
    user_call_list *current_user_call = user->call_list_head;

    // The file the call is to be logged in
    FILE *current_monthly_bill = NULL;

    while (current_user_call != NULL) {

        size_t current_datetime = get_call_node_datetime(current_user_call);

        size_t total_monthly_calls = 0;
        size_t total_monthly_duration = 0;
        double total_mothly_bill = 0;

        while (current_datetime == get_call_node_datetime(current_user_call)) {
            // Calculate monthly stats here
            total_monthly_calls++;
            total_monthly_duration += current_user_call->duration;
            total_mothly_bill += current_user_call->price;

            if (current_user_call->next == NULL) {
                // Set exit flag after the current bill
                current_user_call = current_user_call->next;
                break;
            }
            current_user_call = current_user_call->next;
        }

        // File and file contents generated here

        size_t month = current_datetime % 100;

        char month_string[20];

        switch (month) {
            case JANUARY:
                strcpy(month_string, "January");
                break;
            case FEBRUARY:
                strcpy(month_string, "February");
                break;
            case MARCH:
                strcpy(month_string, "March");
                break;
            case APRIL:
                strcpy(month_string, "April");
                break;
            case MAY:
                strcpy(month_string, "May");
                break;
            case JUNE:
                strcpy(month_string, "June");
                break;
            case JULY:
                strcpy(month_string, "July");
                break;
            case AUGUST:
                strcpy(month_string, "August");
                break;
            case SEPTEMBER:
                strcpy(month_string, "September");
                break;
            case OCTOBER:
                strcpy(month_string, "October");
                break;
            case NOVEMBER:
                strcpy(month_string, "November");
                break;
            case DECEMBER:
                strcpy(month_string, "December");
                break;
            default:
                fprintf(stderr, "Error: Illegal month found, aborting\n");
                exit(1);
                break;
        }
        // Calculate call timecode
        size_t total_call_seconds = calculate_call_seconds(total_monthly_duration);
        size_t total_call_minutes = calculate_call_minutes(total_monthly_duration);
        size_t total_call_hours = calculate_call_hours(total_monthly_duration);

        char *filename = generate_monthly_bill_filename(user->number, current_datetime);

        current_monthly_bill = open_monthly_cdr_bill(filename);
        if (current_monthly_bill == NULL) {
            fprintf(stderr, "Error generating bill for %s %lu for user %s\n", month_string, current_datetime, user->number);
            continue;
        }
        

        fprintf(current_monthly_bill,   "Invoice for %s for Subscriber %s\n"
                                        "Calls: %lu\n"
                                        "Duration: %lu:%02lu:%02lu\n"
                                        "Price: %.2f €", 
                                        month_string, user->number,
                                        total_monthly_calls,
                                        total_call_hours, total_call_minutes, total_call_seconds,
                                        total_mothly_bill);

        free(filename);
        close_monthly_cdr_bill(current_monthly_bill);
        current_monthly_bill = NULL;
    }
}