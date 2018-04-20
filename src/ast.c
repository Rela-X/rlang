#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "ast.h"
#include "utils.h"

Ast *
ast_new(NodeClass c) {
	Ast *m = malloc(sizeof(*m));
	assert(m != NULL);
	m->class = c;
	m->value = NULL;
	m->child = NULL;

	m->scope = NULL;

	m->symbol = NULL;

	m->eval_type = T_NONE;
	m->promoted_type = T_NONE;

	m->next = NULL;

	return m;
}

Ast *
ast_clone(const Ast *ast) {
	assert(ast != NULL);

	Ast *cpy = ast_new(ast->class);
	if(cpy == NULL)
		return NULL;

	if(ast->value != NULL)
		cpy->value = strdup(ast->value);

	for(Ast *c = ast->child; c != NULL; c = c->next) {
		Ast *ccpy = ast_clone(c);
		ast_append_child(cpy, ccpy);
	}

	return cpy;
}

void
ast_append_child(Ast *ast, Ast *child) {
	assert(ast != NULL);
	assert(child != NULL);

	Ast *c = ast->child;
	if(c == NULL) {
		ast->child = child;
	} else {
		while(c->next != NULL)
			c = c->next;
		c->next = child;
	}
}

void _ast_append_child_all(Ast *ast, ...) {
	va_list children;
	va_start(children, ast);
	Ast *child;
	while((child = va_arg(children, Ast *)) != NULL) {
		ast_append_child(ast, child);
	}
	va_end(children);
}

void ast_free(Ast *ast) {
	if(ast) {
		free(ast->value);

		for(Ast *c = ast->child, *next = NULL; c != NULL; c = next) {
			next = c->next;
			ast_free(c);
		}
	}
	free(ast);
}
