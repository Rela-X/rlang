#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "symbol.h"

#include "scope.h"

Symbol *
symbol_new(SymbolClass class, const char *name) {
	Symbol *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->class = class;
	m->name = strdup(name);
	m->eval_type = 0;

	m->assigned = false;
	m->read = false;

	m->args = NULL;
	m->code = NULL;

	m->next = NULL;

	return m;
}

void
symbol_free(Symbol *m) {
	assert(m != NULL);
	assert(m->name != NULL);

	switch(m->class) {
	case S_NONE:
	case S_TYPE:
	case S_VARIABLE:
		break;
	case S_BUILTIN:
	case S_FUNCTION:
		scope_free(m->args);
		break;
	}

	free(m->name);
	free(m);
}

