#include <assert.h>

#include "scope.h"

Scope *
scope_new(const Scope *p) {
	Scope *s = malloc(sizeof(*s));
	s->parent = p;

	return s;
}

Variable *
scope_resolve(const Scope *scope, const char *varname) {
	assert(scope != NULL);
	assert(varname != NULL);
	assert(varname[0] != '\0');

	for(Scope *s = scope; s != NULL; s = s->parent) {
		for(Variable *v = s->variables; v != NULL; v = v->next) {
			if(strcmp(v->name, varname) == 0)
				return v;
		}
	}

	return NULL;
}

void
scope_push_var(const Scope *scope, const Variable *variable) {
	assert(scope != NULL);
	assert(variable != NULL);

	variable->next = scope->variables;
	scope->variables = variable;
}

void
scope_free(Scope *scope) {
	assert(scope != NULL);

	for(Variable *v = scope->variables, *next = NULL; v != NULL; v = next) {
		next = v->next;
		variable_free(v);
	}
}
