#include <stdlib.h>
#include <assert.h>

#include "memory.h"
#include "utils.h"

MemorySpace *
memspace_new(MemorySpace *parent) {
	MemorySpace *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->parent = parent;
	m->mem = NULL;

	return m;
}

Memory *
memspace_load(MemorySpace *memory_space, const Symbol *symbol) {
	for(MemorySpace *ms = memory_space; ms != NULL; ms = ms->parent) {
		for(Memory *m = ms->mem; m != NULL; m = m->next) {
			if(m->symbol == symbol)
				return m;
		}
	}

	return NULL;
}

void
memspace_store(MemorySpace *memory_space, Memory *memory) {
	memory->next = memory_space->mem;
	memory_space->mem = memory;
}

void
memspace_free(MemorySpace *memory_space) {
	Memory *m;
	Memory *next = memory_space->mem;
	while((m = next) != NULL) {
		next = m->next;
		mem_free(m);
	}
	free(memory_space);
}


Memory *
mem_new(Symbol *symbol) {
	Memory *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->symbol = symbol;
	m->value = NULL;
	m->next = NULL;

	return m;
}

void
mem_free(Memory *mem) {
	assert(mem != NULL);
	assert(mem->value != NULL);

	value_free(mem->value);
	free(mem);
}
