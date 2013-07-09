#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "ast.h"

Ast *
ast_new(NodeClass c) {
	Ast *a = malloc(sizeof(*a));
	a->class = c;
	a->value = NULL;
	a->child = NULL;
	a->next = NULL;

	a->eval_type = T_NONE;
	a->promoted_type = T_NONE;

	return a;
}

Ast *
ast_copy(Ast *ast) {
	assert(ast != NULL);

	Ast *cpy = ast_new(ast->class);
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

	switch(ast->class) {
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_IDENTIFIER:
		printf("%s", ast->value);
		break;
	case N_BLOCK:
		printf("do");
		break;
	case N_DECLARESTMT:
		printf("let");
		break;
	case N_IFSTMT:
		printf("if");
		break;
	case N_WHILESTMT:
		printf("while");
		break;
	case N_ASSIGNMENT:
		printf("set");
		break;
	case N_NOT:
		printf("!");
		break;
	case N_EQ:
		printf("==");
		break;
	case N_AND:
		printf("&&");
		break;
	case N_IOR:
		printf("||");
		break;
	case N_XOR:
		printf("!=");
		break;
	case N_LT:
		printf("<");
		break;
	case N_LE:
		printf("<=");
		break;
	case N_GE:
		printf(">=");
		break;
	case N_GT:
		printf(">");
		break;
	case N_NEG:
		printf("-");
		break;
	case N_ADD:
		printf("+");
		break;
	case N_SUB:
		printf("-");
		break;
	case N_MUL:
		printf("*");
		break;
	case N_DIV:
		printf("/");
		break;
	case N_POW:
		printf("**");
		break;
	case N_MOD:
		printf("%%");
		break;
	default:
		printf("%s:%d:%s TODO", __FILE__, __LINE__, __func__);
	}
}

void ast_print_tree(Ast *ast) {
	assert(ast != NULL);

	switch(ast->class) {
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_IDENTIFIER:
		ast_print_node(ast);
		return; // no children
	default:
		;
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
