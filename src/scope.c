#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "scope.h"

Scope *
scope_new(const Scope *p) {
	Scope *s = malloc(sizeof(*s));
	s->parent = p;
	s->symbols = NULL;

	return s;
}

void
scope_define(Scope *scope, Symbol *symbol) {
	assert(scope != NULL);
	assert(symbol != NULL);

	symbol->next = scope->symbols;
	scope->symbols = symbol;
}

Symbol *
scope_resolve(const Scope *scope, const char *name) {
	assert(scope != NULL);
	assert(name != NULL);
	assert(name[0] != '\0');

	for(const Scope *s = scope; s != NULL; s = s->parent) {
		for(Symbol *sy = s->symbols; sy != NULL; sy = sy->next) {
			assert(sy != NULL);
			assert(sy->name != NULL);
			if(strcmp(sy->name, name) == 0)
				return sy;
		}
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
