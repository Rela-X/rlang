#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void annotate_tree(const Ast *);
static void block(const Ast *);
static void function(const Ast *);

void
ast_annotate_scopes(const Ast *ast) {
	printf("setting scope annotations\n");

	annotate_tree(ast);
}

static
void
annotate_tree(const Ast *ast) {
	switch(ast->class) {
	case N_BLOCK:
		block(ast);
		break;
	case N_FUNCTION:
		function(ast);
		break;
	default:
		assert(ast->scope != NULL);
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			c->scope = ast->scope;
			annotate_tree(c);
		}
	}

}

static
void
block(const Ast *block) {
	Scope *s = scope_new(block->scope);

	for(Ast *c = block->child; c != NULL; c = c->next) {
		c->scope = s;
		annotate_tree(c);
	}
}

static
void
function(const Ast *fn) {
	Ast *type = fn->child;
	Ast *id = fn->child->next;
	Ast *args = fn->child->next->next;
	Ast *block = fn->child->next->next->next;

	assert(fn->scope != NULL);

	type->scope = fn->scope;
	id->scope = fn->scope;

	const Scope *global_scope = fn->scope;
	while(global_scope->parent != NULL)
		global_scope = global_scope->parent;

	Scope *s = scope_new(global_scope);
	args->scope = s;
	block->scope = s;

	annotate_tree(args);
	annotate_tree(block);
}
