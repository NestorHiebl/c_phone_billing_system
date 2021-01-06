/**
 *      @file csv_to_linked_list.c
 *      @author Nestor Hiebl
 *      @date December 23, 2020
 *      
 *      @brief The external functions for the csv based phone billing project 
 * 
 *      https://github.com/NestorHiebl/c_phone_billing_system
*/

#include <stdio.h>
#include <stdlib.h>
#include "csv_to_linked_list.h"
#include <ctype.h>

/**
 *      Open CSV
 * 
 *      @brief Handles opening a csv file with a given name.
 * 
 *      @param filename The filename of the csv file.
 * 
 *      @returns The file pointer to given file.
 *      @retval NULL If the file is not found or if handling has failed 
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
 *      Parse rate csv
 * 
 *      The iterative logic for parsing rows and fields is based on @c fgets and @c strtok , respectfully. Potential error scenarions are:
 *      @li A row in the csv is longer than 1024 characters. The function will notify you, but won't attempt to salvage the row.
 *      @li A field is missing. Strtok ignores consecutive delimiters, so any rows with NaN fields will be discarded.
 * 
 *      @brief Builds a full rate linked list based on a csv file pointer
 *      
 *      @param filename The @c FILE pointer for the csv
 * 
 *      @returns A pointer to the head of the generated linked list. Note that no tail is provided.
 */
rate_linked_list *parse_rate_csv(FILE *filename) {
    
    // rate_linked_list *head = NULL;

    char csv_line[1024];

    rate_linked_list *head = NULL;
    rate_linked_list *tail = NULL;

    // Used for debugging
    size_t line_counter = 0;
    while (!(feof(filename))) {
        if ((fgets(csv_line, 1024, filename)) != NULL) {
            
            // Going smoothly, the line has been loaded in successfuly

            if (((csv_line[strlen(csv_line) - 1]) == '\n')) {

                // Remove the trailing newline from the csv row
                printf("Line ended\n");
                csv_line[strlen(csv_line) - 1] = '\0';
                
            } else if (feof(filename)) {
                // We've reached the end of the file
                printf("File ended\n");    
            } else {
                // We're dealing with a really long line
                printf("Line longer than 1024 characters\n");
                continue;  
            } 
                
            char *region_code_token = strtok(csv_line, ",");
            if (region_code_token == NULL) {
                fprintf(stderr, "Line %lu is empty\n", line_counter);
                continue;
            }
            printf("%s, ", region_code_token);
            
            char *region_token = strtok(NULL, ",");
            if (region_token == NULL) {
                fprintf(stderr, "Line %lu is missing two arguments\n", line_counter);
                continue;
            }
            printf("%s, ", region_token);

            char *rate_token = strtok(NULL, ",");
            if (rate_token == NULL) {
                fprintf(stderr, "Line %lu is missing one argument\n", line_counter);
                continue;
            }
            printf("%s ", rate_token);
            
            if (strtok(NULL, ",") != NULL) {
                fprintf(stderr, "Additional field found on line %lu\n", line_counter);
                continue;
            }

            region_code_token = validate_region_code(&region_code_token);
            //double rate = validate_rate(rate_token);   
            
            if ((region_code_token != NULL) && rate_token) {
                
                /*********************************************************
                * The necesarry data has been collected, create the node *
                *********************************************************/
                
                append_rate(&head, &tail, region_code_token, /**** placeholder value ****/ 1.0);

            } else {
                printf("Invalid region_code or rate found on line %lu\n", line_counter);
                continue;
            }
            

        } else {
            // Couldn't load a line in
            fprintf(stderr, "Loading line %lu in csv file failed, aborting\n", line_counter);
            return head;    
        }
        line_counter++;
    }
    return head;
}

/**
 *      Validate region_code
 * 
 *      @brief Checks if a given region_code is legal in E.164 and formats it by removing leading zeros, if need be.
 * 
 *      @param region_code A double pointer to the region_code. May be altered.
 *  
 *      @return A pointer to the valid region_code or NULL
 */
char *validate_region_code(char **region_code){
    if (*region_code == NULL) {
        fprintf(stderr, "Cannot validate NULL string\n");
        return NULL;
    }
    
    // Remove leading zeros
    while (**region_code == '0') {
        *region_code = (*region_code) + 1;
    }
    
    size_t region_code_len = strlen(*region_code);

    if (region_code_len > 11) {
        // region_code too long, invalid
        return NULL;
    }
    

    // Initialize the legality flag as zero to exclude region_codes that don't enter the loop at all (len = 0)
    unsigned short int legal = 0;

    for (size_t i = 0; i < region_code_len; i++) {
        legal = 1;

        if (!(isdigit((*region_code)[i]))) {
            legal = 0;
            break;
        }
    }
    
    // There should be a procedure for comparing every string with every other string to avoid duplicates, this would be a slightly more resource friendly place for that

    if (legal) {
        return *region_code;
    } else {
        return NULL;
    }
    
    
    

}


double validate_rate(char *rate){
    printf("%s\n", rate);
    return 0.0;
}

/*****************************************************************************************************************
 * RATE LINKED LIST FUNCTIONS                                                                                    *
 *****************************************************************************************************************/

/**
 *      Appent rate node
 * 
 *      @brief Appends a new node to the rate linked list. If the head argument is NULL, it initializes a list instead.
 *      
 *      @param head A double pointer to the head of the list, which will be changed dynamically.
 *      @param tail A double pointer to the tail of the list, which will be changed dynamically.
 *      @param region_code The region_code string, cannot be NULL.
 *      @param rate The rate associated with the region_code.
 * 
 *      @returns 1 if the function suceeds, 0 if it fails.
 */
int append_rate(rate_linked_list **head, rate_linked_list **tail, char *region_code, double rate) {

    if (region_code == NULL) {
        fprintf(stderr, "region_code string empty, aborting\n");
        return 0;
    } else if (!(*head == NULL) && ((*tail)->next) != NULL) {
        fprintf(stderr, "Head node is not last node, aborting\n");
        return 0;
    }
    
    rate_linked_list *new_node = malloc(sizeof(rate_linked_list));
    if (new_node == NULL) {
        fprintf(stderr, "Not enough memory to create new node\n");
        return 0;
    }

    // Initialize region_code
    new_node->region_code = malloc(sizeof(region_code));
    if (new_node->region_code == NULL) {
        fprintf(stderr, "Not enough memory to initialize region_code\n");
        return 0;
    } else {
        strcpy(new_node->region_code, region_code);    
    }
    
    new_node->rate = rate;
    
    if (*head == NULL) {
        // The list is being initialized

        new_node->previous = NULL;
        new_node->next = NULL;

        *head = new_node;
        *tail = new_node;
    } else {
        // The node is being appended to an existing list
        (*tail)->next = new_node;

        new_node->previous = *tail;
        new_node->next = NULL;

        *tail = new_node;

        printf("Appended!\n");
    }
    
    return 1;    
}

/**
 *      Print rate list
 * 
 *      @brief Prints a slice of a rate linked list. If both indexes are set to 0, prints the entire list instead.
 * 
 *      @param head The head of the linked list.
 *      @param start_index The starting index of the list slice.
 *      @param end_index The ending index of the list slice.
 */
void print_rate_list(rate_linked_list *head, size_t start_index, size_t end_index) {
    if (head == NULL) {
        fprintf(stderr, "Cannot print NULL list, aborting\n");
        return;
    } else if (start_index > end_index) {
        fprintf(stderr, "Starting index larger than ending index, aborting");
        return;
    }
    
    
    rate_linked_list *current = head;
    if ((start_index == 0) && (end_index == 0)) {
        // If no start and end indexes were given print the whole list

        while (current != NULL) {
        printf("The rate for the region_code \"%s\" equals %f.\n", current->region_code, current->rate);
        current = current->next;
        }    
    } else {
        // Print list from start index to end index
        size_t i = 0;

        while (current != NULL) {
            if (i >= start_index) {
                // If we've reached the start index, print the node
                printf("The rate for the region_code \"%s\" equals %2f.\n", current->region_code, current->rate);
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
 *      Delete rate linked list     
 * 
 *      @brief Deletes a rate linked list, freeing memory.
 * 
 *      @param head A double pointer to the head of the list. Will be set to NULL.
 *      @return 1 if successful, 0 if not. 
 */
int delete_rate_list(rate_linked_list **head) {
    if (*head == NULL) {
        fprintf(stderr, "Cannot delete NULL list, aborting\n");
        return 0;
    }

    rate_linked_list *current = *head;

    while (*head != NULL) {
        current = *head;

        free(current->region_code);                       
        current->region_code = NULL;

        *head = (*head)->next;
        free(current);
        current = NULL;
    }
    
    return 1;
}


// Note - The User list deletition function needs to be nested, because each node contains its own linked list.

/*****************************************************************************************************************
 * AVL TREE FUNCTIONS                                                                                            *
 ****************************************************************************************************************/

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
        printf("New node code smaller than current, going left\n");
        node->left = add_rate_node(node->left, region_code, rate);
    } else if (strcmp(region_code, node->region_code) > 0) {
        // Going right
        printf("New node code larger than current, going right\n");
        node->right = add_rate_node(node->right, region_code, rate);
    } else {
        // This should not happen
        fprintf(stderr, "Error: region code already found in tree\n");
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
    
    newNode->region_code = malloc(sizeof(region_code));
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

    newNode->parent = NULL;
    newNode->left = NULL;
    newNode->right = NULL;

    printf("Created new tree node!\n");
    return newNode;
}

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

void traverse_rates_inorder(rate_node *node, void (*visit) (rate_node*)) {
    if (node == NULL) return;

    traverse_rates_inorder(node->left, visit);
    visit(node);
    traverse_rates_inorder(node->right, visit);
}

void print_rate_node(rate_node *node) {
    if (node == NULL) {
        printf("Node is NULL\n");
        return;
    }
    printf("Key: %s, Rate: %f\n", node->region_code, node->rate);
    return;
}

int get_rate_node_height(rate_node *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;    
}

int get_rate_node_balance(rate_node *node) {
    if (node == NULL) {
        return 0;
    }
    return get_rate_node_height(node->left) - get_rate_node_height(node->right);    
}

int max(int a, int b) {
    return (a > b) ? a : b;
}