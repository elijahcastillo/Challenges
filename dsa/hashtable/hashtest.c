
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to read the next word from a file
char* getNextWord(FILE* file) {
    char buffer[256];
    int index = 0;
    int c;

    // Skip non-alphabetic characters
    while ((c = fgetc(file)) != EOF && !isalpha(c)) {}

    if (c == EOF) {
        return NULL;
    }

    // Read the word
    do {
        buffer[index++] = (char)tolower(c);
        c = fgetc(file);
    } while (c != EOF && isalpha(c));

    buffer[index] = '\0';

    return strdup(buffer);
}

void freeValue(const void* value) {
    free((void*)value);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
			char* key = (ht)->entries[_i].key;				\
			char* value = (ht)->entries[_i].value;				\
			code;	
    const char* filename = argv[1];
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    HashTable ht;
    if (hashTableInit(&ht, 128) == -1) {
        fprintf(stderr, "Error initializing hash table\n");
        fclose(file);
        return EXIT_FAILURE;
    }

    char* word;
    while ((word = getNextWord(file)) != NULL) {
        int* count = (int*)hashTableGet(&ht, word);
        if (count == NULL) {
            count = (int*)malloc(sizeof(int));
            *count = 1;
            hashTableInsert(&ht, word, count);
        } else {
            (*count)++;
            free(word);
        }
    }

    fclose(file);

	hashTableForEach(&ht, key, value, {
		printf("Hash Table Entry: %s -> %d\n", key, *((int*)value));
	});



    hashTableFree(&ht, freeValue);

    return EXIT_SUCCESS;
}
