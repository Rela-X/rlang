#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"
#include "print.h"

static void annotate_tree(Ast *);
static void block(Ast *);
static void function(Ast *);

static const Scope *root_scope;

void
ast_annotate_scopes(Ast *ast, Scope *builtin_scope) {
	printf("setting scope annotations\n");

	ast->scope = builtin_scope;
	root_scope = builtin_scope;

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
	block->scope = scope_new(block->scope);

	for(Ast *c = block->child; c != NULL; c = c->next) {
		c->scope = block->scope;
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
	while(global_scope->parent != root_scope)
		global_scope = global_scope->parent;

	Scope *parameter_scope = scope_new(global_scope);
	args->scope = parameter_scope;
	block->scope = parameter_scope;

	annotate_tree(args);
	annotate_tree(block);
}
