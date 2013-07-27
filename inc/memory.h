#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdbool.h>

#include "symbol.h"

union _value {
	bool as_bool;
	int as_int;
	float as_float;
	char *as_str;
	/*
	rf_Set *as_Set;
	rf_Relation *as_R;
	*/
};
typedef union _value Value;

typedef struct _memory Memory;
struct _memory {
	Symbol          *symbol;
	Value           value;

	Memory          *next;
};

typedef struct _memory_space MemorySpace;
struct _memory_space {
	MemorySpace     *parent;
	Memory          *mem;
};

MemorySpace *   memspace_new(const MemorySpace *parent);
Memory *        memspace_load(const MemorySpace *, const Symbol *);
void            memspace_store(MemorySpace *, Memory *);
void            memspace_free(MemorySpace *);

Memory *        mem_new(Symbol *);
void            mem_free(Memory *);

#endif
