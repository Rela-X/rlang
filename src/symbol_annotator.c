#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "scope.h"

static void annotate_function_symbols(Ast *);
static void annotate_variable_symbols(Ast *);
static void function(Ast *, Ast *, Ast *, Ast *);
static void annotate_return_stmts(Ast *, Symbol *);
static void functionargs(Ast *);
static void declaration(Ast *, Ast *);
static void assignment(Ast *, Ast *);
static void identifier(Ast *);
static void annotate_type_symbol(Ast *);
static void annotate_identifier_symbol(Ast *);
static void define_symbol(const Ast *, SymbolClass);
static inline Symbol *resolve_symbol(const Ast *);
static inline Symbol *resolve_symbol_recursive(const Ast *);

void
ast_annotate_symbols(Ast *ast) {
	printf("setting and validating symbol annotations\n");

	annotate_function_symbols(ast);
	annotate_variable_symbols(ast);
}

static
void
annotate_function_symbols(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_FUNCTION:
		function(ast->child, ast->child->next, ast->child->next->next, ast->child->next->next->next);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			annotate_function_symbols(c);
		}
	}
}

static
void
annotate_variable_symbols(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_FUNCTIONARGS:
		functionargs(ast);
		break;
	case N_DECLARATION:
		declaration(ast->child, ast->child->next);
		break;
	case N_ASSIGNMENT:
		assignment(ast->child, ast->child->next);
		break;
	case N_IDENTIFIER:
		identifier(ast);
		assert(ast->symbol != NULL);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			annotate_variable_symbols(c);
		}
	}
}

static
void
function(Ast *type, Ast *id, Ast *args, Ast *block) {
	annotate_type_symbol(type);

	define_symbol(id, S_FUNCTION);
	annotate_identifier_symbol(id);

	id->symbol->args = args->scope;
	id->symbol->code = block;

	annotate_return_stmts(block, id->symbol);

	annotate_function_symbols(block);
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
void
functionargs(Ast *fnargs) {
	for(Ast *c = fnargs->child; c != NULL; c = c->next) {
		annotate_variable_symbols(c);
		c->child->next->symbol->assigned = true;
	}
}

static
void
declaration(Ast *type, Ast *id) {
	annotate_type_symbol(type);

	define_symbol(id, S_VARIABLE);
	annotate_identifier_symbol(id);
}

static
void
assignment(Ast *id, Ast *expr) {
	/* check expression first */
	annotate_variable_symbols(expr);

	annotate_identifier_symbol(id);
	assert(id->symbol != NULL);
	id->symbol->assigned = true;
}

static
void
identifier(Ast *id) {
	annotate_identifier_symbol(id);

	if(id->symbol->class == S_VARIABLE && !id->symbol->assigned) {
		printf("use of unassigned identifier <%s>!\n", id->value);
	}
}

static
void
annotate_type_symbol(Ast *type) {
	type->symbol = resolve_symbol_recursive(type);
	if(type->symbol == NULL || type->symbol->class != S_TYPE) {
		printf("<%s> is not a valid type!\n", type->value);
		assert(false);
	}
}

static
void
annotate_identifier_symbol(Ast *id) {
	id->symbol = resolve_symbol_recursive(id);
	if(id->symbol == NULL) {
		printf("<%s> not found!\n", id->value);
		assert(false);
	}
}

static
void
define_symbol(const Ast *id, SymbolClass class) {
	Symbol *sy;
	/* check if identifier is a reserved word */
	sy = resolve_symbol_recursive(id);
	if(sy != NULL && sy->class == S_TYPE) {
		printf("<%s> is not a valid name!\n", id->value);
		assert(false);
	}
	/* check if identifier is already defined in the current (!) scope */
	sy = resolve_symbol(id);
	if(sy != NULL) {
		printf("<%s> already defined!\n", id->value);
		assert(false);
	}

	/* define symbol */
	scope_define(id->scope, symbol_new(class, id->value));
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
