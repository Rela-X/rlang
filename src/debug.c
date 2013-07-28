#include <stdio.h>

#include "ast.h"

void
debug_tree(const Ast *ast) {
	ast_print_tree(ast);
	printf("\n");
}

void
debug_node(const Ast *ast) {
	ast_print_node(ast);
	printf("\n");
}

void
debug_symbol(const Ast *ast) {
	Symbol *symbol = ast->symbol;

	ast_print_node(ast);
	printf(" has symbol <%s>", symbol->name);
	printf("(");
	printf("assigned=%s|", (symbol->assigned) ? "yes" : "NO!");
	printf("read=%s", (symbol->read) ? "yes" : "NO!");
	printf(")\n");
}

void
debug_symbols(const Ast *ast) {
	debug_symbol(ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		debug_symbols(c);
	}
}

void
debug_type(const Ast *ast) {
	ast_print_node(ast);
	printf(" has type ");
	print_type(ast->eval_type);
	if(ast->promoted_type != T_NONE) {
		printf("(promoted to ");
		print_type(ast->promoted_type);
		printf(")");
	}
	printf("\n");
}

void
debug_types(const Ast *ast) {
	debug_type(ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		debug_types(c);
	}
}
