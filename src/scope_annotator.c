#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void annotate_tree(Ast *);
static void block(Ast *);
static void function(Ast *);

static const *builtin_scope;

void
ast_annotate_scopes(Ast *ast) {
	printf("setting scope annotations\n");

	builtin_scope = ast->scope;

	annotate_tree(ast);
}

static
void
annotate_tree(Ast *ast) {
	assert(ast->scope != NULL);
	switch(ast->class) {
	case N_BLOCK:
		block(ast);
		break;
	case N_FUNCTION:
		function(ast);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			c->scope = ast->scope;
			annotate_tree(c);
		}
	}

}

static
void
block(Ast *block) {
	Scope *s = scope_new(block->scope);

	for(Ast *c = block->child; c != NULL; c = c->next) {
		c->scope = s;
		annotate_tree(c);
	}
}

static
void
function(Ast *fn) {
	Ast *type = fn->child;
	Ast *id = fn->child->next;
	Ast *args = fn->child->next->next;
	Ast *block = fn->child->next->next->next;

	type->scope = fn->scope;
	id->scope = fn->scope;

	Scope *global_scope = fn->scope;
	while(global_scope->parent != builtin_scope)
		global_scope = global_scope->parent;

	Scope *s = scope_new(global_scope);
	args->scope = s;
	block->scope = s;

	annotate_tree(args);
	annotate_tree(block);
}
