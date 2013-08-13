#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdbool.h>

#include "symbol.h"
#include "value.h"

typedef struct _memory Memory;
struct _memory {
	Symbol          *symbol;
	Value           *value;

	Memory          *next;
};

typedef struct _memory_space MemorySpace;
struct _memory_space {
	MemorySpace     *parent;
	Memory          *mem;
};

MemorySpace *   memspace_new(MemorySpace *parent);
Memory *        memspace_load(MemorySpace *, const Symbol *);
void            memspace_store(MemorySpace *, Memory *);
void            memspace_free(MemorySpace *);

Memory *        mem_new(Symbol *);
void            mem_free(Memory *);

#endif
