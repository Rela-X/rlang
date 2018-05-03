#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "scope.h"

static int annotate_function_symbols(Ast *);
static int annotate_variable_symbols(Ast *);
static int function(Ast *, Ast *, Ast *, Ast *);
static void annotate_return_stmts(Ast *, Symbol *);
static int functionargs(Ast *);
static int declaration(Ast *, Ast *);
static int assignment(Ast *, Ast *);
static int identifier(Ast *);
static bool type_symbol_ok(Ast *);
static bool identifier_symbol_ok(Ast *);
static int define_symbol(const Ast *, SymbolClass);
static inline Symbol *resolve_symbol(const Ast *);
static inline Symbol *resolve_symbol_recursive(const Ast *);

int
ast_annotate_symbols(Ast *ast) {
	int err = 0;
	if((err = annotate_function_symbols(ast))) return err;
	if((err = annotate_variable_symbols(ast))) return err;
	return err;
}

static
int
annotate_function_symbols(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_FUNCTION:
		return function(ast->child, ast->child->next, ast->child->next->next, ast->child->next->next->next);
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			int err = 0;
			if((err = annotate_function_symbols(c))) return err;
		}
	}

	return 0;
}

static
int
annotate_variable_symbols(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_FUNCTIONARGS:
		return functionargs(ast);
	case N_DECLARATION:
		return declaration(ast->child, ast->child->next);
	case N_ASSIGNMENT:
		return assignment(ast->child, ast->child->next);
	case N_IDENTIFIER:
		return identifier(ast);
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			int err = 0;
			if((err = annotate_variable_symbols(c))) return err;
		}
	}

	return 0;
}

static
int
function(Ast *type, Ast *id, Ast *args, Ast *block) {
	type->symbol = resolve_symbol_recursive(type);
	if(!type_symbol_ok(type)) {
		return 1;
	}

	if(define_symbol(id, S_FUNCTION)) {
		return 1;
	}

	id->symbol = resolve_symbol_recursive(id);
	if(!identifier_symbol_ok(id)) {
		return 1;
	}

	id->symbol->args = args->scope;
	id->symbol->code = block;

	annotate_return_stmts(block, id->symbol);

	return annotate_function_symbols(block);
}

static
void
annotate_return_stmts(Ast *ast, Symbol *sy) {
	switch(ast->class) {
	case N_FUNCTION:
		// ignore nested function-blocks
		break;
	case N_RETURN:
		ast->symbol = sy;
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			annotate_return_stmts(c, sy);
		}
	}
}

static
int
functionargs(Ast *fnargs) {
	int err = 0;
	for(Ast *c = fnargs->child; c != NULL; c = c->next) {
		if((err = annotate_variable_symbols(c))) return err;
		c->child->next->symbol->assigned = true;
	}

	return 0;
}

static
int
declaration(Ast *type, Ast *id) {
	type->symbol = resolve_symbol_recursive(type);
	if(!type_symbol_ok(type)) {
		return 1;
	}

	if(define_symbol(id, S_VARIABLE)) {
		return 1;
	}
	id->symbol = resolve_symbol_recursive(id);
	if(!identifier_symbol_ok(id)) {
		return 1;
	}

	return 0;
}

static
int
assignment(Ast *id, Ast *expr) {
	/* check expression first */
	int err = 0;
	if((err = annotate_variable_symbols(expr))) return err;

	id->symbol = resolve_symbol_recursive(id);
	if(!identifier_symbol_ok(id)) {
		return 1;
	}
	id->symbol->assigned = true;

	return 0;
}

static
int
identifier(Ast *id) {
	id->symbol = resolve_symbol_recursive(id);
	if(!identifier_symbol_ok(id)) {
		return 1;
	}

	if(id->symbol->class == S_VARIABLE && !id->symbol->assigned) {
		printf("use of unassigned identifier <%s>!\n", id->value);
	}

	return 0;
}

static
bool
type_symbol_ok(Ast *type) {
	if(type->symbol == NULL || type->symbol->class != S_TYPE) {
		printf("<%s> is not a valid type!\n", type->value);
		return false;
	}
	return true;
}

static
bool
identifier_symbol_ok(Ast *id) {
	if(id->symbol == NULL) {
		printf("<%s> not found!\n", id->value);
		return false;
	}
	return true;
}

static
int
define_symbol(const Ast *id, SymbolClass class) {
	Symbol *sy;
	/* check if identifier is a reserved word */
	sy = resolve_symbol_recursive(id);
	if(sy != NULL && sy->class == S_TYPE) {
		printf("<%s> is not a valid name!\n", id->value);
		return 1;
	}
	/* check if identifier is already defined in the current (!) scope */
	sy = resolve_symbol(id);
	if(sy != NULL) {
		printf("<%s> already defined!\n", id->value);
		return 1;
	}

	/* define symbol */
	scope_define(id->scope, symbol_new(class, id->value));

	return 0;
}

static inline
Symbol *
resolve_symbol(const Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);
	return sy;
}

static inline
Symbol *
resolve_symbol_recursive(const Ast *id) {
	Symbol *sy = scope_resolve_recursive(id->scope, id->value);
	return sy;
}
