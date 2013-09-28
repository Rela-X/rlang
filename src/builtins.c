#include <stdio.h>
#include <stdarg.h>

#include "ast.h"
#include "memory.h"
#include "print.h"

typedef Value *(* builtin_function)(Scope *, MemorySpace *);

static
Value *
get_value_by_name(Scope *args, MemorySpace *memspace, char *name) {
	Symbol *sy = scope_resolve(args, name);
	Memory *mem = memspace_load(memspace, sy);

	return mem->value;
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

void
init_builtin_functions(Scope *builtin_scope) {
	define_builtin_function(builtin_scope, T_VOID, "print", &builtin_print, 1, T_STRING);
}
