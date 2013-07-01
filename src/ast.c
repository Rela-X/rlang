#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "ast.h"

Ast *
ast_new(NodeType t) {
	Ast *a = malloc(sizeof(*a));
	a->type = t;
	a->value = NULL;
	a->child = NULL;
	a->next = NULL;

	return a;
}

Ast *
ast_copy(Ast *ast) {
	assert(ast != NULL);

	Ast *cpy = ast_new(ast->type);
	if(cpy == NULL)
		return NULL;

	if(ast->value != NULL)
		cpy->value = strdup(ast->value);

	for(Ast *c = ast->child; c != NULL; c = c->next) {
		Ast *ccpy = ast_copy(c);
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

void ast_print_node(Ast *ast) {
	assert(ast != NULL);

	switch(ast->type) {
	case NT_BOOLEAN:
	case NT_INTEGER:
	case NT_FLOAT:
	case NT_STRING:
	case NT_IDENTIFIER:
		printf("%s", ast->value);
		break;
	case NT_BLOCK:
		printf("do");
		break;
	case NT_DECLARESTMT:
		printf("let");
		break;
	case NT_IFSTMT:
		printf("if");
		break;
	case NT_WHILESTMT:
		printf("while");
		break;
	case NT_ASSIGNMENT:
		printf("set");
		break;
	case NT_NOT:
		printf("!");
		break;
	case NT_EQ:
		printf("==");
		break;
	case NT_AND:
		printf("&&");
		break;
	case NT_IOR:
		printf("||");
		break;
	case NT_XOR:
		printf("!=");
		break;
	case NT_LT:
		printf("<");
		break;
	case NT_LE:
		printf("<=");
		break;
	case NT_GE:
		printf(">=");
		break;
	case NT_GT:
		printf(">");
		break;
	case NT_NEG:
		printf("-");
		break;
	case NT_ADD:
		printf("+");
		break;
	case NT_SUB:
		printf("-");
		break;
	case NT_MUL:
		printf("*");
		break;
	case NT_DIV:
		printf("/");
		break;
	case NT_POW:
		printf("**");
		break;
	case NT_MOD:
		printf("%");
		break;
	default:
		printf("%s:%d:%s TODO", __FILE__, __LINE__, __func__);
	}
}

void ast_print_tree(Ast *ast) {
	assert(ast != NULL);

	switch(ast->type) {
	case NT_BOOLEAN:
	case NT_INTEGER:
	case NT_FLOAT:
	case NT_STRING:
	case NT_IDENTIFIER:
		ast_print_node(ast);
		return; // no children
	}

	printf("(");
	ast_print_node(ast);
	printf(" ");
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		ast_print_tree(c);
		if(c->next != NULL)
			printf(" ");
	}
	printf(")");
}

void ast_free(Ast *ast) {
	assert(ast != NULL);

	for(Ast *c = ast->child, *next = NULL; c != NULL; c = next) {
		next = c->next;
		ast_free(c);
	}
	free(ast);
}
