#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "scope.h"

Scope *
scope_new(Scope *p) {
	Scope *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->parent = p;
	m->symbols = NULL;

	if(p != NULL) {
		m->free_ref = p->free_ref;
		p->free_ref = m;
	} else {
		m->free_ref = NULL;
	}

	return m;
}

void
scope_define(Scope *scope, Symbol *symbol) {
	assert(scope != NULL);
	assert(symbol != NULL);

	/* a scope serves double-duty as the argument
	 * list of a function. Symbols must be stored
	 * in the order in which they were defined.
	 */
	if(scope->symbols == NULL) {
		scope->symbols = symbol;
	} else {
		Symbol *last = scope->symbols;
		while(last->next != NULL)
			last = last->next;
		last->next = symbol;
	}
}

Symbol *
scope_resolve(const Scope *scope, const char *name) {
	assert(scope != NULL);
	assert(name != NULL);
	assert(name[0] != '\0');

	for(Symbol *sy = scope->symbols; sy != NULL; sy = sy->next) {
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

	for(Symbol *sy = scope->symbols, *next = NULL; sy != NULL; sy = next) {
		next = sy->next;
		symbol_free(sy);
	}
}
