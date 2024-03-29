#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

char INVALID_ERR[] = "Invalid Input Format\n";



int checkTokenQuotes(char *t) {
    /*
     * Checks for double quotes around a given string t.
     * Returns:
     * 0 if the token is not quoted
     * 1 if the token is quoted
     * -1 if the token contains invalid quotes (e.g. only on one end of the token)
     */

    int length = strlen(t);

    if (length == 0)
        return 0;

    if (length == 1 && t[0] == '\"') {
        return -1;
    }

    if ((t[0] == '\"' && t[length - 1] != '\"') ||  // Check for mismatching quotes
        ((t[0] != '\"' && t[length - 1] == '\"'))) {
        return -1;
    }

    if (t[0] == '\"')
        return 1;
    else
        return 0;
}

/* To store the counts of each tweeter, we used a linked list
    where each node contains the name of the tweeter and the number
    of tweets they made so far in the file. Allocating nodes in a linked_list
    is easier than reallocating space in an array. */

struct Node {
    char* name;
    int count;
    struct Node *next;
};

typedef struct Node* Node_t;

struct linked_list {
    int size;
    Node_t head;
};

typedef struct linked_list* linked_list_t;

linked_list_t linked_list_create(void) {
    /*
     * Creates an empty linked list
     *
     * Returns:
     * A pointer to the new linked list
     */
    linked_list_t l = (linked_list_t) malloc(sizeof(struct linked_list));
    if (l == NULL) {
        return NULL;
    }
    l->head = NULL;
    l->size = 0;

    return l;
}

int linked_list_insert(char* name, linked_list_t lizt) {
    /*
     * Searches the linked list for a node with the same name as the input
     * and if found increments that node's count, else creates a new node in
     * the list
     *
     * Arguments:
     * name: string of name we are searching for
     * lizt: the linked list
     * Returns:
     * 0 if the name is in the list
     *
     */
    Node_t current = lizt->head;
    int found = 0;

    // Go through the list to find the node with the exacy na,e
    while (current != NULL && !found) {
        if(!strcmp(current->name, name)) {
            found = 1;
            current->count++;
        }
        current = current->next;
    }

    // If a node wasnt found, append a new node to the list with that na,e
    if (!found) {
        Node_t new_node = (Node_t) malloc(sizeof(struct Node));
        new_node->next = lizt->head;
        new_node->name = name;
        new_node->count = 1;
        lizt->head = new_node;
        lizt->size++;
    }

    return 0;
}

void linked_list_free(linked_list_t l) {
    /*
     * Free up the nodes in the list
     *
     * Arguments:
     * l: the linked list
     */
    Node_t curr = l->head;
    Node_t next = NULL;
    while (curr) {
        next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }

    free(l);
}

void linked_list_print(linked_list_t l, int numToPrint) {
    /*
     * Prints the info of the first numToPrint nodes in the linked list
     *
     * Arguments:
     * l: the linked list
     * numToPrint: integer for how many nodes to print
     */
    Node_t curr = l->head;
    char *name = NULL;
    int length = 0;
    int count = 0;

    while (curr != NULL && count < numToPrint) {
        name = curr->name;

        // If the name is quoted, print the unquoted version
        if (checkTokenQuotes(name) > 0) {
            name++;
            length = strlen(name);
            if (length >= 1)
                name[length - 1] = '\0';
        }

        printf("%s: %d\n", name, curr->count);
        curr = curr->next;
        count++;
    }

    return;
}

void node_split(Node_t old, Node_t* left, Node_t* right) {
    /*
     * Helper fn of mergesort which splits two linked lists in half
     *
     * Arguments:
     * old: node pointer of the head node of the list to be split
     * left: node pointer which will store the head pointer for the left half
     * right: node pointer which will store the head pointer for the right half
     */
    Node_t fast;
    Node_t slow;
    slow = old;
    fast = old->next;

    // Slow pointer will reach the middle when fast reaches the end
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // Set left list to the original head and set the right list to
    //  the middle+1 node and sever the link between the lists
    *left = old;
    *right = slow->next;
    slow->next = NULL;
}

Node_t merge(Node_t left, Node_t right) {
    /*
     * Helper fn of mergesort which merges two linked lists in sorted order
     *
     * Arguments:
     * left: node pointer for the left sublist
     * right: node pointer for the right sublist
     *
     * Returns:
     * A node pointer to the merged sorted list of left and right
     */
    Node_t ret = NULL;

    // Base case if one of the lists is NULL
    if (left == NULL)
        return right;
    else if (right == NULL)
        return left;

    // Check each first node of the list and then recursively call merge
    if (left->count > right->count) {
        ret = left;
        ret->next = merge(left->next, right);
    }
    else {
        ret = right;
        ret->next = merge(left, right->next);
    }

    // Return the merged list
    return ret;
}

Node_t linked_list_mergesort(Node_t* head) {
    /*
     *  Implementation of mergesort on the linked list whose head is passed in
     *
     * Arguments:
     * head: node pointer to the head of the linked list
     *
     * Returns:
     * A node pointer to the sorted list
     */

    // Base case
    if (((*head) == NULL) || ((*head)->next == NULL)) {
        return *head;
    }

    Node_t left;
    Node_t right;

    // Split the list in half and recursively call mergesort on both halves
    node_split(*head, &left, &right);
    left = linked_list_mergesort(&left);
    right = linked_list_mergesort(&right);

    // return the linked list after merging bboth sorted halves
    return merge(left, right);
}

void linked_list_sort(linked_list_t lizt) {
    /*
     * Sorts the linked list passed in
     *
     * Arguments:
     * lizt: pointer to the linked list
     */
    lizt->head = linked_list_mergesort(&(lizt->head));
}

void printError(char *err) {
    /*
     * Prints the error message passed in and exits the program
     *
     * Arguments:
     * err: string of the error message
     */
    printf("%s", err);
    exit(1);
}

int *processCSVHeader(char **header, int numColumns, int *nameIndex) {
    /*
     * Processes an array of strings as the headers of a csv
     *
     * Arguments:
     * header: array of strings representing header names
     * numColumns: number of columns in the header
     * nameIndex: pointer used to store the index of the "name" header.
     *              If NULL, no value is stored.
     *
     * Returns:
     * Array of boolean values representing whether the column at each index
     *  is quoted or not
     */

    int *isQuoted = (int *) malloc(sizeof(int)*numColumns);
    int isNameFound = 0;

    if (numColumns == 0) {  // Check if header column is empty
        printError(INVALID_ERR);
    }

    for (int i = 0; header[i]; i++) {
        if (!strcmp(header[i], "name") || !strcmp(header[i], "\"name\"")) {
            if (isNameFound) {  // Check if there is a duplicate name column
                printError(INVALID_ERR);
            }
            isNameFound = 1;
            if (nameIndex)
                *nameIndex = i;
        }

        isQuoted[i] = checkTokenQuotes(header[i]);
        if (isQuoted[i] == -1) {
            printError(INVALID_ERR);
            exit(1);
        }
    }

    if (!isNameFound) { // Check if header is missing name column
        printError(INVALID_ERR);
        exit(1);
    }

    return isQuoted;
}

char** split(char* str, char c, int *numSubstr) {
    /*
     * Split str based by char c.
     *
     * Arguments:
     * str: the string to split
     * c: the char to split the string by
     * numSubstr: pointer used to store the number of substrings found.
     *             If NULL, no value is stored.
     *
     * Returns:
     * Array of malloc'd substrings, terminated by a NULL pointer.
     */

    int numOccurrences = 0;
    for (int i = 0; str[i] != '\0'; i++) {  // Find all occurences of c
        if (str[i] == c)
            numOccurrences++;
    }

    char **substrs = (char **) malloc(sizeof(char *)*(numOccurrences + 2));
    int start = 0;

    int i = 0;
    while (start == 0 || str[start - 1] != '\0') {
        int length = 0;

        for (int j = start; str[j] != '\0' && str[j] != c; j++)
            length++;

        // allocate additional char for the null character
        substrs[i] = (char *) malloc(sizeof(char)*(length + 1));
        strncpy(substrs[i], str + start, length);
        substrs[i][length] = '\0';

        start += length + 1;
        i++;
    }

    substrs[numOccurrences + 1] = NULL;

    if (numSubstr)
        *numSubstr = numOccurrences + 1;

    return substrs;
}

int main(int argc, char **argv )
{
    if (argc != 2) {    // Check command line inputs
        int n = strlen(basename(argv[0]));
        char s[n + 21];
        sprintf(s, "Usage: %s path/to/csv\n", basename(argv[0]));
        printError(s);
    }

    FILE* stream = fopen(argv[1], "r");
    if (stream == NULL) {
        printError("Failed to open input file\n");
    }

    linked_list_t lizt = linked_list_create();

    int lineNumber = 1;
    int name_index = -1;

    char *line = NULL;
    char *name = NULL;
    int *isHeaderQuoted = NULL;

    size_t length = 0;
    int read = 0;
    int numColumns = 0;
    int numHeaderColumns = 0;

    // Process Header first
    read = getline(&line, &length, stream);

    if (read > 1024) {  // Check if header is less than 1024 characters
        printError(INVALID_ERR);
    }

    if (line[read - 1] == '\n')
        line[read - 1] = '\0'; // get rid of newline

    char **col_values = split(line, ',', &numHeaderColumns);

    isHeaderQuoted = processCSVHeader(col_values, numHeaderColumns, &name_index);

    while ((read = getline(&line, &length, stream)) != -1)
    {
        if (read > 1024) {  // Check if header is less than 1024 characters
            printError(INVALID_ERR);
        }

        lineNumber++;
        if (lineNumber > 20000) {
            printError(INVALID_ERR);
        }

        if (line[read - 1] == '\n')
            line[read - 1] = '\0'; // get rid of newline

        col_values = split(line, ',', &numColumns);

        if (numColumns != numHeaderColumns) {
             // Check that each row has same number of columns as header
            printError(INVALID_ERR);
        }

        for (int i = 0; col_values[i] != NULL; i++) {

                int isQuoted = 0;

                isQuoted = checkTokenQuotes(col_values[i]);
                if (isHeaderQuoted[i] != isQuoted) {
                    // Check that value is quoted if and only if header is quoted
                    printError(INVALID_ERR);
                }

                if (i == name_index) {
                    name = col_values[name_index];
                }
                else {  // free unneeded column values
                    free(col_values[i]);
                }

                if (name != NULL) {
                    linked_list_insert(name, lizt);
                }
                name = NULL;
        }

        free(col_values);
    }

    linked_list_sort(lizt);
    linked_list_print(lizt, 10);    // Print first 10 items in the list

    fclose(stream);

    linked_list_free(lizt);

    return 0;
}
