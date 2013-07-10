#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void walk_tree(Ast *);
static void declaration(const Ast *);
static Symbol * identifier(Ast *);
static inline Symbol * id_resolve_symbol(Ast *);

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
	case N_DECLARESTMT:
		declaration(ast);
		break;
	case N_IDENTIFIER:
		ast->symbol = identifier(ast);
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
declaration(const Ast *declaration) {
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;
	Ast *expr = declaration->child->next->next;

	if(expr != NULL) {
		/* expr must be checked first to catch constructs like
		 * int a = a + 0;
		 */
		walk_tree(expr);
	}

	type->symbol = id_resolve_symbol(type);
	if(type->symbol == NULL || type->symbol->class != S_TYPE) {
		printf("<%s> is not a valid type!\n", type->value);
	}

	Symbol *sy;
	sy = id_resolve_symbol(id);
	if(sy != NULL) {
		printf("<%s>", id->value);
		switch(sy->class) {
		case S_TYPE:
			printf(" is not a valid name!\n");
			break;
		case S_VARIABLE:
			printf(" already defined!\n");
			break;
		}
	}

	sy = symbol_new(id->value);
	scope_define(id->scope, sy);

	walk_tree(id);
}

static
Symbol *
identifier(Ast *id) {
	Symbol *sy = id_resolve_symbol(id);
	if(sy == NULL) {
		printf("<%s> not found!\n", id->value);
	}

	return sy;
}

static inline
Symbol *
id_resolve_symbol(Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);
	return sy;
}
