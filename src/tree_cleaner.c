#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "scope.h"

static
void
block(Ast *block) {
	scope_free(block->scope);
}

static
void
walk_tree(Ast *ast) {
	switch(ast->class) {
	case N_BLOCK:
		block(ast);
		break;
	}

	for(Ast *c = ast->child; c != NULL; c = c->next) {
		walk_tree(c);
	}

}

void
ast_cleanup(Ast *ast) {
	walk_tree(ast);
}
