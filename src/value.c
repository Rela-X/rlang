#include <stdlib.h>
#include <assert.h>

#include "value.h"

static void value_reset(Value *);

Value *
value_new() {
	Value *m = malloc(sizeof(*m));
	m->type = T_NONE;

	return m;
}

void
value_copy(Value *dst, Value *src) {
	assert(dst != NULL);
	assert(src != NULL);

	value_reset(dst);

	switch(src->type) {
	case T_NONE:
	case T_VOID:
		break;
	case T_BOOL:
		dst->as_bool = src->as_bool;
		break;
	case T_INT:
		dst->as_int = src->as_int;
		break;
	case T_FLOAT:
		dst->as_float = src->as_float;
		break;
	case T_STRING:
		dst->as_String = strdup(src->as_String);
		break;
	}
	dst->type = src->type;
}

void
value_set_bool(Value *v, bool nv) {
	v->type = T_BOOL;
	v->as_bool = nv;
}

void
value_set_int(Value *v, int nv) {
	v->type = T_INT;
	v->as_int = nv;
}

void
value_set_float(Value *v, double nv) {
	v->type = T_FLOAT;
	v->as_float = nv;
}

void
value_set_string(Value *v, char *nv) {
	v->type = T_STRING;
	v->as_String = strdup(nv);
}

static
void
value_reset(Value *v) {
	switch(v->type) {
	case T_NONE:
	case T_VOID:
	case T_BOOL:
	case T_INT:
	case T_FLOAT:
		break;
	case T_STRING:
		free(v->as_String);
		break;
	}
	v->type = T_NONE;
}

void
value_free(Value *v) {
	assert(v != NULL);

	value_reset(v);

	free(v);
}
