#include <stdio.h>
#include <assert.h>

#include "types.h"

static char *type_name_table[] = {
	[T_VOID] = "void",
	[T_BOOL] = "boolean",
	[T_INT] = "integer",
	[T_FLOAT] = "float",
	[T_STRING] = "String",
	[T_R] = "Relation",
};

void
print_type(Type type) {
	printf("type(%d)", type);

	assert(type >= 0);
	assert(type < NTYPES);

	printf(" => %s", type_name_table[type]);
}

