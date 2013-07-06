#include <stdio.h>
#include <assert.h>

#include "types.h"

static char *type_name_table[] = {
	[rl_VOID] = "void",
	[rl_BOOL] = "boolean",
	[rl_INT] = "integer",
	[rl_FLOAT] = "float",
	[rl_STRING] = "String",
	[rl_R] = "Relation",
};

void
print_type(RLangType type) {
	printf("type(%d) => %s", type, type_name_table[type]);
}

