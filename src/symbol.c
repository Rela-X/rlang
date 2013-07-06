#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbol.h"

Symbol *
symbol_new(const char *name) {
	Symbol *m = malloc(sizeof(*m));
	m->name = strdup(name);
	m->eval_type = 0;

	return m;
}

void
symbol_free(Symbol *m) {
	assert(m != NULL);

	free(m->name);
	free(m);
}
