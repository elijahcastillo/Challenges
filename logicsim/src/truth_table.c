#include "truth_table.h"
#include <stdlib.h>

TruthTable* truthTableInit(int inputs, int outputs) {
    TruthTable* tt = (TruthTable*)malloc(sizeof(TruthTable));
    if (tt == NULL) return NULL;

    tt->numInputs = inputs;
    tt->numOutputs = outputs;
    tt->maxEntries = 1 << inputs;  // Compute 2^inputs using bit shifting
    tt->size = 0;
    tt->entries = (TTEntry*)malloc(tt->maxEntries * sizeof(TTEntry));
    if (tt->entries == NULL) {
        free(tt);
        return NULL;
    }

    return tt;
}

void truthTableAddEntry(TruthTable* tt, uint32_t inp, uint32_t out) {
    if (tt->size < tt->maxEntries) {
        tt->entries[tt->size].inp = inp;
        tt->entries[tt->size].out = out;
        tt->size++;
    } else {
    }
}

uint32_t truthTableFind(TruthTable* tt, uint32_t inp) {
    for (size_t i = 0; i < tt->size; i++) {
        if (tt->entries[i].inp == inp) {
            return tt->entries[i].out;
        }
    }
    return 0; // Default to 0 if no entry is found
}

void truthTableFree(TruthTable* tt) {
    free(tt->entries);
    free(tt);
}
