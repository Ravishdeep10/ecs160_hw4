#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

char** split(char* str, char c) {

    int numCommas = 0;
    for (int i = 0; i < str[i] != '\0'; i++) {
        if (str[i] == c) {
            numCommas++;
        }
    }

    char **substrs = (char **) malloc(sizeof(char *)*(numCommas + 1));
    int i = 0;
    while (str[i] != '\0') {
        int start = i, end = 0, length = 0;
        for (int j = i; str[j] != '\0'; j++) {
            if (str[j] == c) {
                end = j;
                break;
            }
        }
        length = end - start + 1;
        substrs[i] = (char *) malloc(sizeof(char)*(length + 1));
        strncpy(str + i, substrs[i], length);
        substrs[i][length] = '\0';
        i = end + 1;
    }

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

    char line[1024];
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        printf("Field 3 would be %s\n", getfield(tmp, 3));
        // NOTE strtok clobbers tmp
        free(tmp);
    }    char line[1024];
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        printf("Field 3 would be %s\n", getfield(tmp, 3));
        // NOTE strtok clobbers tmp
        free(tmp);
    }
    
    fclose(stream);
    
    return 0;
}


