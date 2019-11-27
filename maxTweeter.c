#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

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

linked_list_t linked_list_create(void)
{
    linked_list_t l = (linked_list_t) malloc(sizeof(struct linked_list));
    if (l == NULL) {
        return NULL;
    }
    l->head = NULL;
    l->size = 0;
    
    return l;
}

int linked_list_insert(char* name, linked_list_t lizt)
{
    Node_t current = lizt->head;
    int found = 0;
    while (current != NULL && !found) {
        if(!strcmp(current->name, name)) {
            found = 1;
            current->count++;
        }
        current = current->next;
    }
    
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

void split(Node_t old, Node_t* left, Node_t* right) {
    Node_t fast;
    Node_t slow;
    slow = old;
    fast = old->next;
    
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *left = old;
    *right = slow->next;
    slow->next = NULL;
}

Node_t merge(Node_t left, Node_t right) {
    Node_t ret = NULL;
    
    if (left == NULL)
        return right;
    else if (right == NULL)
        return left;
    
    if (left->count <= right->count) {
        ret = left;
        ret->next = merge(left->next, right);
    }
    else {
        ret = right;
        ret->next = merge(left, right->next);
    }
    return ret;
}

Node_t mergesort(Node_t* head) {
    if (((*head) == NULL) || ((*head)->next == NULL)) {
        return head;
    }
    
    Node_t left;
    Node_t right;
    
    split(*head, &left, &right);
    
    left = mergesort(&left);
    right = mergesort(&right);
    
    return merge(left, right);
}


void linked_list_sort(linked_list_t lizt) {
    lizt->head = mergesort(&(lizt->head));
}

int checkTokenQuotes(char *t) {
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

int *processCSVHeader(char **header, int numColumns, int *nameIndex) {
    int *isQuoted = (int *) malloc(sizeof(int)*numColumns);
    int isNameFound = 0;

    if (numColumns == 0) {  // Check if header column is empty
        printError();
        exit(1);
    }

    for (int i = 0; header[i]; i++) {
        if (!strcmp(header[i], "name") || !strcmp(header[i], "\"name\"")) {
            if (isNameFound) {  // Check if there is a duplicate name column
                printError();
                exit(1);
            }
            isNameFound = 1;
            if (nameIndex)
                *nameIndex = i;
        }

        isQuoted[i] = checkTokenQuotes(header[i]);
        if (isQuoted[i] == -1) {
            printError();
            exit(1);
        }
    }

    if (!isNameFound) { // Check if header is missing name column
        printError();
        exit(1);
    }

    return isQuoted;
}

char** split(char* str, char c, int *numSubstr) {

    int numOccurrences = 0;
    for (int i = 0; str[i] != '\0'; i++) {  // Find all occurences of c
        if (str[i] == c) 
            numOccurrences++;
    }

    char **substrs = (char **) malloc(sizeof(char *)*(numOccurrences + 2));
    int start = 0, end = 0;

    int i = 0;
    while (start == 0 || str[start - 1] != '\0') {
        int length = 0;

        for (int j = start; str[j] != '\0' && str[j] != c; j++) 
            length++;

        substrs[i] = (char *) malloc(sizeof(char)*(length + 1));    // allocate additional char for the null character
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

void printError() {
    printf("Invalid Input Format\n");
}

int main(int argc, char **argv )
{
    if (argc != 2) {
        printError();
        return 1;
    }
    
    FILE* stream = fopen(argv[1], "r");
    if (stream == NULL) {
        printError();
        return 1;
    }
    
    linked_list_t lizt = linked_list_create();
    
    int lineNumber = 1;
    int name_index = -1;
    
    char *line = NULL;
    char *name = NULL;
    int *isHeaderQuoted = NULL;

    // try size_t instead of int if doesnt work
    size_t length = 0;
    int read = 0;
    int numColumns = 0;
    int numHeaderColumns = 0;

    // Process Header first
    read = getline(&line, &length, stream);

    if (read > 1024) {
        printError();
        exit(1);
    }

    line[read - 1] = '\0'; // get rid of newline

    char **col_values = split(line, ',', &numHeaderColumns);

    isHeaderQuoted = processCSVHeader(line, numHeaderColumns, &name_index);

    while ((read = getline(&line, &length, stream)) != -1)
    {
        if (read > 1024) {
            printError();
            return 1;
        }

        line[read - 1] = '\0'; // get rid of newline
        
        col_values = split(line, ',', &numColumns);

        if (numColumns != numHeaderColumns) {
            printError();
            exit(1);
        }

        for (int i = 0; col_values[i] != NULL; i++) {

                int isQuoted = 0;

                isQuoted = checkTokenQuotes(col_values[i]);
                if (isQuoted != isHeaderQuoted[i]) {
                    printError();
                    exit(1);
                }

                if (i == name_index) {
                    name = col_values[name_index];
                } 
                else {
                    free(col_values[i]);
                }

                if (name != NULL) {
                    linked_list_insert(name, lizt);
                }
        }
        
        free(col_values);

        // Clean up time
        name = NULL;
    }
    
    
    linked_list_sort(lizt);

    // TODO
    // Enable parsing for quoted header values
    // Sort linked list (quicksort?)
    // Grab and print top 3 results
        
    fclose(stream);

    linked_list_free(lizt);
    
    return 0;
}


