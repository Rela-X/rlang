#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "ast.h"

Token *
token_new(int type, char *value) {
	Token *t = malloc(sizeof(*t));
	t->type = type;
	t->value = strdup(value);

	return t;
}

void token_free(Token *t) {
	assert(t != NULL);

	free(t->value);
	free(t);
}

Ast *
ast_new(Token *t) {
	Ast *a = malloc(sizeof(*a));
	a->token = t;
	a->child = NULL;
	a->next = NULL;

	return a;
}

void
ast_append_child(Ast *ast, Ast *child) {
	assert(ast != NULL);
	assert(child != NULL);

printf("%s:%d:%s appending ", __FILE__, __LINE__, __func__);
ast_print_node(child);

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

void ast_print_node(Ast *ast) {
	assert(ast != NULL);

	printf("<%s>\n", (ast->token != NULL) ? ast->token->value : "null");
}

void ast_print_tree(Ast *ast) {
	assert(ast != NULL);

	if(ast->child == NULL) {
		printf("<%s>\n", (ast->token != NULL) ? ast->token->value : "null");
	} else {
		printf("(%s", (ast->token != NULL) ? ast->token->value : "null");
		for(Ast *c = ast->child, *next = NULL; c != NULL; c = next) {
			ast_print_tree(c);
		}
	}
}

void ast_free(Ast *ast) {
	assert(ast != NULL);

	for(Ast *c = ast->child, *next = NULL; c != NULL; c = next) {
		next = c->next;
		ast_free(c);
	}
	if(ast->token != NULL) token_free(ast->token);
	free(ast);
}
