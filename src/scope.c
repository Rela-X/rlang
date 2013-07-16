#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "scope.h"

Scope *
scope_new(const Scope *p) {
	Scope *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->parent = p;
	m->symbol = NULL;

	return m;
}

void
scope_define(Scope *scope, Symbol *symbol) {
	assert(scope != NULL);
	assert(symbol != NULL);

	symbol->next = scope->symbol;
	scope->symbol = symbol;
}

Symbol *
scope_resolve(const Scope *scope, const char *name) {
	assert(scope != NULL);
	assert(name != NULL);
	assert(name[0] != '\0');

	for(Symbol *sy = scope->symbol; sy != NULL; sy = sy->next) {
		assert(sy->name != NULL);
		if(strcmp(sy->name, name) == 0)
			return sy;
	}

	return NULL;
}

Symbol *
scope_resolve_recursive(const Scope *scope, const char *name) {
	assert(scope != NULL);
	assert(name != NULL);
	assert(name[0] != '\0');

	for(const Scope *s = scope; s != NULL; s = s->parent) {
		Symbol *sy = scope_resolve(s, name);
		if(sy != NULL)
			return sy;
	}

	return NULL;
}

void
scope_free(Scope *scope) {
	assert(scope != NULL);

	for(Symbol *sy = scope->symbol, *next = NULL; sy != NULL; sy = next) {
		next = sy->next;
		symbol_free(sy);
	}
}
