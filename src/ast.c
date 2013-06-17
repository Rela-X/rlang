#include <stdlib.h>
#include <string.h>

#include "ast.h"

Token *
token_new(int type, char *value) {
	Token *t = malloc(sizeof(*t));
	t->type = type;
	t->value = strdup(value);

	return t;
}

void token_free(Token *t) {
	free(t->value);
	free(t);
}

Ast *
ast_new(Token *t) {
	Ast *a = malloc(sizeof(*a));
	a->token = t;

	return a;
}

void ast_print_node(Ast *ast) {
	printf("<%s>\n", ast->token->value);
}

void ast_print_tree(Ast *ast) {
	printf("TODO: ast_to_string_tree\n");
}

void ast_free(Ast *ast) {
	for(Ast *c = *ast->children; c != NULL; c++) {
		ast_free(c);
	}
	if(ast->token != NULL) token_free(ast->token);
	free(ast);
}
