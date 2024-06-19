#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_INIT_CAP 10

static size_t defaultHashFunc(const char* key) {
    size_t hashValue = 0;
    while (*key) {
        hashValue = (hashValue * 31) + *key++;
    }
    return hashValue;
}

int _hashTableResize(HashTable* ht, size_t newCapacity){
	KeyVal* newEntries = calloc(newCapacity, sizeof(KeyVal));
	if(!newEntries) return -1;

	// Keep track of old values for insertion
    size_t oldCapacity = ht->entriesCap;
    KeyVal* oldEntries = ht->entries;

	// Reset for new insertion
    ht->entries = newEntries;
    ht->entriesCap = newCapacity;
    ht->entriesSize = 0;

    for (size_t i = 0; i < oldCapacity; i++) {
        if (oldEntries[i].key != NULL) {
            // Rehash the entries into the new table
            hashTableInsert(ht, oldEntries[i].key, oldEntries[i].value);

			// Free the old key after rehashing
            free(oldEntries[i].key); 
        }
    }
	
    free(oldEntries);
    return 0;
}

int hashTableInit(HashTable* ht, size_t initialCapacity){
	ht->entriesCap = initialCapacity <= 0 ? HASH_TABLE_INIT_CAP : initialCapacity;
	ht->entriesSize = 0;
	ht->entries = calloc(ht->entriesCap, sizeof(KeyVal));
	if(!ht->entries){
		ht = NULL;
		return -1;
	} 
	ht->hash = defaultHashFunc;
	return 0;

}


void  hashTableSetHashFunc(HashTable* ht, size_t (*hash_func)(const char* key)){
	ht->hash = hash_func;
}


int hashTableInsert(HashTable* ht, char* key, void* value){
	if(key == NULL) return -1;

	if(ht->entriesSize >= ht->entriesCap){
		if(_hashTableResize(ht, ht->entriesCap * 2) == -1) return -1; // Resize Failed
	}

	size_t hash = ht->hash(key) % ht->entriesCap;
	while(ht->entries[hash].key != NULL && strcmp(ht->entries[hash].key, key) != 0){
		hash = (hash + 1) % ht->entriesCap;
	}

    ht->entries[hash].key = strdup(key);
    ht->entries[hash].value = value;
	ht->entriesSize++;
	return 0;
};

void* hashTableGet(HashTable* ht, char* key){
	if(key == NULL) return NULL; // Invalid Key

	size_t hash = ht->hash(key) % ht->entriesCap;
	while(ht->entries[hash].key != NULL){
		if (strcmp(ht->entries[hash].key, key) == 0){
			return ht->entries[hash].value;
		}
		hash = (hash + 1) % ht->entriesCap;
	}

	return NULL; // Did not find key
}


int hashTableDelete(HashTable* ht, char* key){
	if(key == NULL) return -1;

    size_t hash = ht->hash(key) % ht->entriesCap;
    while (ht->entries[hash].key != NULL) {
        if (strcmp(ht->entries[hash].key, key) == 0) {
            free(ht->entries[hash].key);
            ht->entries[hash].key = NULL;
            ht->entries[hash].value = NULL;
            ht->entriesSize--;
            return 0;
        }
        hash = (hash + 1) % ht->entriesCap;
    }

	return -1;
}

void  hashTableFree(HashTable* ht, void (*value_free)(const void* value)){
    for (size_t i = 0; i < ht->entriesCap; i++) {
        if (ht->entries[i].key != NULL) {
            free(ht->entries[i].key);
            if (value_free != NULL) {
                value_free(ht->entries[i].value);
            }
        }
    }
    free(ht->entries);
}
