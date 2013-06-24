#include <assert.h>

#include "variable.h"

Variable *
variable_new(const char *name) {
	Variable *v = malloc(sizeof(*v));
	v->name = name;
}

void
variable_free(Variable *var) {
	assert(var != NULL);

	free(var->name);
	free(var);
}
