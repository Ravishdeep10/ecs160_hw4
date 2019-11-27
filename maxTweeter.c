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

int main(int argc, char **argv )
{
    if (argc != 2) {
        printf("Usage: %s csvfilepath\n", basename(argv[0]));
        return 1;
    }
    
    FILE* stream = fopen(argv[1], "r");
    if (stream == NULL) {
        fprintf(stderr, "Error opening file: %d\n", errno);
        return 1;
    }
    
    linked_list_t lizt = linked_list_create();
    
    int name_index = -1;
    int tweet_index = -1;
    
    int header_line = 0;
    int found_text = 0;
    int found_name = 0;
    
    char* line = NULL;
    char *name = NULL;
    char *tweet = NULL;

    // try size_t instead of int if doesnt work
    size_t length = 0;
    int read = 0;
    int numColumns = 0;
    int numHeaderColumns = 0;
    while ((read = getline(&line, &length, stream)) != -1)
    {
        if (read > 1024) {
            fprintf(stderr, "Error csv contains lines of length 1024+: %d\n", errno);
            return 1;
        }
        
        char** col_values = split(line, ',', &numColumns);
        int i = 0;
        
        if (!header_line) {
            numHeaderColumns = numColumns;
        } else if (numColumns != numHeaderColumns) {
            fprintf(stderr, "Error: Invalid Formatting\n");
            exit(1);
        }
        
        while (col_values[i] != NULL) {

            if (!header_line) {

                if (!strcmp(col_values[i], "text")) {
                    tweet_index = i;
                    found_text = 1;
                }
                
                if (!strcmp(col_values[i], "name")) {
                    name_index = i;
                    found_name = 1;
                }

                header_line = found_text & found_name;
            }
            else if (header_line){
                if (i == name_index) {
                    name = col_values[name_index];
                }

                if (i == tweet_index) {
                    tweet = col_values[tweet_index];
                }
            }

            if (tweet != NULL && name != NULL && strlen(tweet) > 0) {
                linked_list_insert(name, lizt);
            }

            i++;
        }
        
        // Clean up time
        name = NULL;
        tweet = NULL;
        
        if (!header_line || numHeaderColumns == 0) {
            fprintf(stderr, "Error: Invalid Formatting\n");
            return 1;
        }
    }
    
    
    linked_list_sort(lizt);

    // TODO
    // Enable parsing for quoted header values
    // Safely free malloc'd strings from split()
    // Sort linked list (quicksort?)
    // Grab and print top 3 results
        
    fclose(stream);
    
    return 0;
}


