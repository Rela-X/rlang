#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"

static void annotate_tree(Ast *);

void
ast_annotate_scopes(const Ast *ast) {
	printf("setting scope annotations\n");

	annotate_tree(ast);
}

static
void
annotate_tree(Ast *ast) {
	assert(ast->scope != NULL);

	Scope *current_scope = ast->scope;
	switch(ast->class) {
	case N_BLOCK:
		current_scope = scope_new(current_scope);
		// fall through
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) { 
			c->scope = current_scope;
			annotate_tree(c);
		}
	}
}

