#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "ast.h"
#include "memory.h"
#include "print.h"

typedef Value *(* builtin_function)(Scope *, MemorySpace *);

static
Value *
get_value_by_name(Scope *args, MemorySpace *memspace, char *name) {
	Symbol *sy = scope_resolve(args, name);
	Memory *mem = memspace_load(memspace, sy);

	assert(sy->eval_type == mem->value->type);

	return mem->value;
}

static
void
define_builtin_function(Scope *builtin_scope, Type rtype, char *name, builtin_function fn, int nparams, ...) {
	Symbol *fn_symbol = symbol_new(S_BUILTIN, name);
	fn_symbol->eval_type = rtype;
	fn_symbol->fn = fn;

	scope_define(builtin_scope, fn_symbol);

	fn_symbol->args = scope_new(NULL);
	va_list params;
	va_start(params, nparams);
	for(int i = 0; i < nparams; i++) {
		Type t = va_arg(params, Type);

		/* the n-th parameter is called "n" */
		char buf[32];
		snprintf(buf, 32, "%d", i);

		Symbol *sy = symbol_new(S_VARIABLE, buf);
		sy->eval_type = t;
		scope_define(fn_symbol->args, sy);
	}
	va_end(params);
}

static
Value *
builtin_print(Scope *args, MemorySpace *memspace) {
	Value *text = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	printf("%s\n", text->as_String);
	value_set_void(rval);

	return rval;
}

static
Value *
builtin_bool2str(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_string(rval, strdup((v->as_bool) ? "true" : "false"));

	return rval;
}

static
Value *
builtin_int2str(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	char buf[64];
	snprintf(buf, 64, "%d", v->as_int);
	value_set_string(rval, strdup(buf));

	return rval;
}

static
Value *
builtin_float2str(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	char buf[64];
	snprintf(buf, 64, "%f", v->as_float);
	value_set_string(rval, strdup(buf));

	return rval;
}

static
Value *
builtin_set2str(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_string(rval, rf_set_to_string(v->as_Set));

	return rval;
}

static
Value *
builtin_r2str(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_string(rval, rf_relation_to_string(v->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_homogeneous(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_homogeneous(v->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_reflexive(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_reflexive(v->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_symmetric(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_symmetric(v->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_antisymmetric(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_antisymmetric(v->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_transitive(Scope *args, MemorySpace *memspace) {
	Value *v = get_value_by_name(args, memspace, "0");
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_transitive(v->as_Relation));

	return rval;
}

void
init_builtin_functions(Scope *builtin_scope) {
	define_builtin_function(builtin_scope, T_VOID, "print", &builtin_print, 1, T_STRING);

	define_builtin_function(builtin_scope, T_STRING, "bool2str", &builtin_bool2str, 1, T_BOOL);
	define_builtin_function(builtin_scope, T_STRING, "int2str", &builtin_int2str, 1, T_INT);
	define_builtin_function(builtin_scope, T_STRING, "float2str", &builtin_float2str, 1, T_FLOAT);
	define_builtin_function(builtin_scope, T_STRING, "set2str", &builtin_set2str, 1, T_SET);
	define_builtin_function(builtin_scope, T_STRING, "r2str", &builtin_r2str, 1, T_R);

	define_builtin_function(builtin_scope, T_BOOL, "relation_is_homogeneous", &builtin_relation_is_homogeneous, 1, T_R);
	define_builtin_function(builtin_scope, T_BOOL, "relation_is_reflexive", &builtin_relation_is_reflexive, 1, T_R);
	define_builtin_function(builtin_scope, T_BOOL, "relation_is_symmetric", &builtin_relation_is_symmetric, 1, T_R);
	define_builtin_function(builtin_scope, T_BOOL, "relation_is_antisymmetric", &builtin_relation_is_antisymmetric, 1, T_R);
	define_builtin_function(builtin_scope, T_BOOL, "relation_is_transitive", &builtin_relation_is_transitive, 1, T_R);
}
