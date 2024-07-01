#ifndef TRUTH_TABLE_H
#define TRUTH_TABLE_H
#include <stdint.h>
#include <stdlib.h>

typedef struct {
	uint32_t inp;	
	uint32_t out; 
} TTEntry;

typedef struct {
	int numInputs;
	int numOutputs;
	size_t maxEntries;
	size_t size;
	TTEntry* entries;
} TruthTable;


TruthTable* truthTableInit(int inputs, int outputs);
void truthTableAddEntry(TruthTable* tt, uint32_t inp, uint32_t out);
uint32_t truthTableFind(TruthTable* tt, uint32_t inp);
void truthTableFree(TruthTable* tt);


#endif // TRUTH_TABLE_H
