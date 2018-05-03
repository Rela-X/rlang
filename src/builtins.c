#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include <relafix/set.h>
#include <relafix/relation.h>
#include <relafix/text_io.h>

#include "ast.h"
#include "memory.h"
#include "print.h"
#include "utils.h"

#define VA_NUM_ARGS(...) \
	(sizeof(#__VA_ARGS__) == sizeof("") ? 0 : VA_NUM_ARGS_IMPL(__VA_ARGS__, 9,8,7,6,5,4,3,2,1,0))
#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,N,...) \
	N

typedef Value *(* builtin_function)(Scope *, MemorySpace *);

static
Value *
get_value_by_name(Scope *args, MemorySpace *memspace, char *name) {
	Symbol *sy = scope_resolve(args, name);
	Memory *mem = memspace_load(memspace, sy);

	assert(sy->eval_type == mem->value->type);

	return mem->value;
}

#define def_function(rtype, name, fnptr, ...) \
	_define_builtin_function(builtin_scope, rtype, name, fnptr, VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__);
static
void
_define_builtin_function(Scope *builtin_scope, Type rtype, char *name, builtin_function fn, int nparams, ...) {
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
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	printf("%s\n", arg[0]->as_String);
	value_set_void(rval);

	return rval;
}

static
Value *
builtin_bool2str(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_string(rval, strdup((arg[0]->as_bool) ? "true" : "false"));

	return rval;
}

static
Value *
builtin_int2str(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	char buf[64];
	snprintf(buf, 64, "%d", arg[0]->as_int);
	value_set_string(rval, strdup(buf));

	return rval;
}

static
Value *
builtin_float2str(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	char buf[64];
	snprintf(buf, 64, "%f", arg[0]->as_float);
	value_set_string(rval, strdup(buf));

	return rval;
}

static
Value *
builtin_set2str(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_string(rval, rf_set_to_string(arg[0]->as_Set));

	return rval;
}

static
Value *
builtin_r2str(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_string(rval, rf_relation_to_string(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_set_new_intersection(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_set(rval, rf_set_new_intersection(arg[0]->as_Set, arg[1]->as_Set));

	return rval;
}

static
Value *
builtin_set_new_powerset(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_set(rval, rf_set_new_powerset(arg[0]->as_Set));

	return rval;
}

static
Value *
builtin_set_is_subset(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_set_is_subset(arg[0]->as_Set, arg[1]->as_Set));

	return rval;
}

static
Value *
builtin_relation_new_union(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_relation(rval, rf_relation_new_union(arg[0]->as_Relation, arg[1]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_new_intersection(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_relation(rval, rf_relation_new_intersection(arg[0]->as_Relation, arg[1]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_new_concatenation(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_relation(rval, rf_relation_new_concatenation(arg[0]->as_Relation, arg[1]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_new_complement(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_relation(rval, rf_relation_new_complement(arg[0]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_new_converse(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_relation(rval, rf_relation_new_converse(arg[0]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_is_homogeneous(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_homogeneous(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_antisymmetric(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_antisymmetric(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_asymmetric(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_asymmetric(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_difunctional(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_difunctional(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_equivalent(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_equivalent(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_irreflexive(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_irreflexive(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_partial_order(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_partial_order(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_preorder(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_preorder(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_reflexive(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_reflexive(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_symmetric(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_symmetric(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_transitive(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_transitive(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_lattice(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_lattice(arg[0]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_is_sublattice(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
		get_value_by_name(args, memspace, "1"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_sublattice(arg[0]->as_Relation, arg[1]->as_Relation, NULL));

	return rval;
}

static
Value *
builtin_relation_is_lefttotal(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_lefttotal(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_functional(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_functional(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_function(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_function(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_surjective(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_surjective(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_injective(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_injective(arg[0]->as_Relation));

	return rval;
}

static
Value *
builtin_relation_is_bijective(Scope *args, MemorySpace *memspace) {
	Value *arg[] = {
		get_value_by_name(args, memspace, "0"),
	};
	Value *rval = value_new();

	value_set_bool(rval, rf_relation_is_bijective(arg[0]->as_Relation));

	return rval;
}

void
init_builtin_functions(Scope *builtin_scope) {
	/* basic functions */
	def_function(T_VOID, "print", &builtin_print, T_STRING);

	/* conversion functions */
	def_function(T_STRING, "bool2str", &builtin_bool2str, T_BOOL);
	def_function(T_STRING, "int2str", &builtin_int2str, T_INT);
	def_function(T_STRING, "float2str", &builtin_float2str, T_FLOAT);
	def_function(T_STRING, "set2str", &builtin_set2str, T_SET);
	def_function(T_STRING, "r2str", &builtin_r2str, T_R);

	/* set functions */
	def_function(T_SET, "set_intersection", &builtin_set_new_intersection, T_SET, T_SET);
	def_function(T_SET, "set_powerset", &builtin_set_new_powerset, T_SET);

	def_function(T_BOOL, "set_is_subset", &builtin_set_is_subset, T_SET, T_SET);

	/* relation functions */
	def_function(T_R, "relation_union", &builtin_relation_new_union, T_R, T_R);
	def_function(T_R, "relation_intersection", &builtin_relation_new_intersection, T_R, T_R);
	def_function(T_R, "relation_concatenation", &builtin_relation_new_concatenation, T_R, T_R);
	def_function(T_R, "relation_complement", &builtin_relation_new_complement, T_R);
	def_function(T_R, "relation_converse", &builtin_relation_new_converse, T_R);

	def_function(T_BOOL, "relation_is_homogeneous", &builtin_relation_is_homogeneous, T_R);
	def_function(T_BOOL, "relation_is_antisymmetric", &builtin_relation_is_antisymmetric, T_R);
	def_function(T_BOOL, "relation_is_asymmetric", &builtin_relation_is_asymmetric, T_R);
	def_function(T_BOOL, "relation_is_difunctional", &builtin_relation_is_difunctional, T_R);
	def_function(T_BOOL, "relation_is_equivalent", &builtin_relation_is_equivalent, T_R);
	def_function(T_BOOL, "relation_is_irreflexive", &builtin_relation_is_irreflexive, T_R);
	def_function(T_BOOL, "relation_is_partial_order", &builtin_relation_is_partial_order, T_R);
	def_function(T_BOOL, "relation_is_preorder", &builtin_relation_is_preorder, T_R);
	def_function(T_BOOL, "relation_is_reflexive", &builtin_relation_is_reflexive, T_R);
	def_function(T_BOOL, "relation_is_symmetric", &builtin_relation_is_symmetric, T_R);
	def_function(T_BOOL, "relation_is_transitive", &builtin_relation_is_transitive, T_R);
	def_function(T_BOOL, "relation_is_lattice", &builtin_relation_is_lattice, T_R);
	def_function(T_BOOL, "relation_is_sublattice", &builtin_relation_is_sublattice, T_R, T_R);
	def_function(T_BOOL, "relation_is_lefttotal", &builtin_relation_is_lefttotal, T_R);
	def_function(T_BOOL, "relation_is_functional", &builtin_relation_is_functional, T_R);
	def_function(T_BOOL, "relation_is_function", &builtin_relation_is_function, T_R);
	def_function(T_BOOL, "relation_is_surjective", &builtin_relation_is_surjective, T_R);
	def_function(T_BOOL, "relation_is_injective", &builtin_relation_is_injective, T_R);
	def_function(T_BOOL, "relation_is_bijective", &builtin_relation_is_bijective, T_R);
}
