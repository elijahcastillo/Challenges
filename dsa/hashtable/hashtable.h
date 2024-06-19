#ifndef HASH_TABLE_H
#define HASH_TABLE_H
#include <stddef.h>

typedef struct KeyVal {
	char* key;
	void* value;
} KeyVal;


typedef struct {
	KeyVal* entries;
	size_t entriesSize;
	size_t entriesCap;
	size_t (*hash)(const char* key);
} HashTable;


/*
 * Allows you to easily loop over all of the entries
 * in the hash table and run any code on each
 * Example:
 *	hashTableForEach(&ht, key, value, {
		printf("Hash Table Entry: %s -> %d\n", key, *((int*)value));
	});
 */
#define hashTableForEach(ht, key, value, code)			\
	do {												\
		for (int _i = 0; _i < (ht)->entriesCap; _i++) {	\
			if((ht)->entries[_i].key != NULL){			\
				char* key = (ht)->entries[_i].key;		\
				char* value = (ht)->entries[_i].value;	\
				code;									\
			}											\
														\
		}												\
	} while (0)											\

/* 
 * Initializes a already allocated HashTable struct
 * If initial capacity is <= 0, HASH_TABLE_INIT_CAP will be used
 * Returns 0 on success and -1 on error
 */
int hashTableInit(HashTable* ht, size_t initialCapacity);

/*
 * Allows you to set your own hash function for the key
 * A default hash table is set on initialization
 */
void  hashTableSetHashFunc(HashTable* ht, size_t (*hash_func)(const char* key));

/*
 * Inserts a key value pair into the hash table
 * If key is NULL OR if resizing of the table fails it returns -1
 * Returns 0 on success
 */
int  hashTableInsert(HashTable* ht, char* key, void* value);

/*
 *	Retrives a value from the hash table associated with the provided key
 *	If key == NULL OR no entries exists with that key it returns NULL
 */
void* hashTableGet(HashTable* ht, char* key);

/*
 * Deletes a key value pair from the tables entries
 * If key == NULL OR key value pair is not found it returns -1
 * Returns 0 on success
 */
int  hashTableDelete(HashTable* ht, char* key);

/*
 * Free the allocated thats part of the hash table
 * If the values in the table dont need to be explicietly freed
 * pass in NULL as the second argument: hashTableFree(ht, NULL)
 */
void  hashTableFree(HashTable* ht, void (*value_free)(const void* value));


#endif // HASH_TABLE_H
