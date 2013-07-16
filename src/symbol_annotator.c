#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void walk_tree(Ast *);
static void declaration(Ast *, Ast *, Ast *);
static void assignment(Ast *, Ast *);
static void identifier(Ast *);
static void annotate_identifier(Ast *id);
static inline Symbol *resolve_symbol(const Ast *);
static inline Symbol *resolve_symbol_recursive(const Ast *);

void
ast_annotate_symbols(Ast *ast) {
	printf("setting and validating symbol annotations\n");

	walk_tree(ast);
}

static
void
walk_tree(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_DECLARATION:
		declaration(ast->child, ast->child->next, ast->child->next->next);
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
			walk_tree(c);
		}
	}
}

static
void
declaration(Ast *type, Ast *id, Ast *expr) {
	type->symbol = resolve_symbol_recursive(type);
	if(type->symbol == NULL || type->symbol->class != S_TYPE) {
		printf("<%s> is not a valid type!\n", type->value);
	}

	Symbol *sy;
	/* check if identifier is a reserved word */
	sy = resolve_symbol_recursive(id);
	if(sy != NULL && sy->class == S_TYPE) {
		printf("<%s> is not a valid name!\n", id->value);
	}
	/* check if identifier is already defined in the current (!) scope */
	sy = resolve_symbol(id);
	if(sy != NULL && sy->class == S_VARIABLE) {
		printf("<%s> already defined!\n", id->value);
	}

	/* define symbol */
	scope_define(id->scope, symbol_new(id->value));

	if(expr != NULL) {
		assignment(id, expr);
	} else {
		annotate_identifier(id);
	}
}

static
void
assignment(Ast *id, Ast *expr) {
	/* check expression first */
	walk_tree(expr);

	annotate_identifier(id);
	id->symbol->assigned = true;
}

static
void
identifier(Ast *id) {
	annotate_identifier(id);
	if(!id->symbol->assigned) {
		printf("use of unassigned identifier <%s>!\n", id->value);
	}
}

static
void
annotate_identifier(Ast *id) {
	id->symbol = resolve_symbol_recursive(id);
	if(id->symbol == NULL) {
		printf("<%s> not found!\n", id->value);
	}
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
