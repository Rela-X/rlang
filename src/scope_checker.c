#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void check_block(Ast *);
static void check_declaration(Ast *);
static void check_identifier(Ast *);

void
check_symbols(Ast *ast) {
	assert(ast->scope != NULL);

	Scope *current_scope = ast->scope;
	switch(ast->type) {
	case NT_BLOCK:
		check_block(ast);
		assert(ast->scope != ast->child->scope);
		break;
	case NT_DECLARESTMT:
		check_declaration(ast);
		break;
	case NT_IDENTIFIER:
		check_identifier(ast);
		assert(ast->symbol != NULL);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) { 
			c->scope = current_scope;
			check_symbols(c);
		}
	}
}

static
void
check_block(Ast *block) {
	Scope *current_scope = block->scope;
	Scope *new = scope_new(current_scope);
	for(Ast *c = block->child; c != NULL; c = c->next) { 
		c->scope = new;
		check_symbols(c);
	}
	//scope_free(new); // TODO where to free scopes?
}

static
void
check_declaration(Ast *declaration) {
	Scope *current_scope = declaration->scope;
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;
	Ast *expr = declaration->child->next->next;

	type->scope = current_scope;
	check_symbols(type);

	Symbol *sy;
	sy = scope_resolve(current_scope, id->value);
	if(sy != NULL)
		printf("<%s> already defined", sy->name);

	id->scope = current_scope;
	id->symbol = symbol_new(id->value);
	scope_define(current_scope, id->symbol);

	if(expr == NULL)
		return;
	expr->scope = current_scope;
	check_symbols(expr);
}

static
void
check_identifier(Ast *id) {
	Scope *current_scope = id->scope;
	Symbol *sy = scope_resolve(current_scope, id->value);
	if(sy == NULL) {
		printf("<%s> not found!\n", id->value);
	}
	id->symbol = sy;
}

