#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "value.h"

static void value_reset(Value *);

Value *
value_new() {
	Value *m = malloc(sizeof(*m));
	m->type = T_NONE;
	value_reset(m);

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
	case T_SET:
		dst->as_Set = rf_set_clone(src->as_Set);
		break;
	case T_R:
		dst->as_Relation = rf_relation_clone(src->as_Relation);
		break;
	}
	dst->type = src->type;
}

void
value_set_void(Value *v) {
	value_reset(v);
	v->type = T_VOID;
}

void
value_set_bool(Value *v, bool nv) {
	value_reset(v);
	v->type = T_BOOL;
	v->as_bool = nv;
}

void
value_set_int(Value *v, int nv) {
	value_reset(v);
	v->type = T_INT;
	v->as_int = nv;
}

void
value_set_float(Value *v, double nv) {
	value_reset(v);
	v->type = T_FLOAT;
	v->as_float = nv;
}

void
value_set_string(Value *v, char *nv) {
	value_reset(v);
	v->type = T_STRING;
	v->as_String = nv;
}

void
value_set_set(Value *v, rf_Set *nv) {
	value_reset(v);
	v->type = T_SET;
	v->as_Set = nv;
}

void
value_set_relation(Value *v, rf_Relation *nv) {
	value_reset(v);
	v->type = T_R;
	v->as_Relation = nv;
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
	case T_SET:
		rf_set_free(v->as_Set);
		break;
	case T_R:
		rf_relation_free(v->as_Relation);
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
