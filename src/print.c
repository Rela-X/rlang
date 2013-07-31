#include <assert.h>

#include "print.h"
#include "types.h"

static char *node_name_table[] = {
	[N_BLOCK] = "do", 
	[N_DECLARATION] = "decl", 
	[N_FUNCTION] = "function", 
	[N_FUNCTIONARGS] = "fnargs", 
	[N_IF] = "if", 
	[N_WHILE] = "while", 
	[N_ASSIGNMENT] = "set", 
	[N_CALL] = "call", 
	[N_CALLARGS] = "cargs", 
	[N_NOT] = "!", 
	[N_EQ] = "==", 
	[N_AND] = "&&", 
	[N_IOR] = "||", 
	[N_XOR] = "!=", 
	[N_LT] = "<", 
	[N_LE] = "<=", 
	[N_GE] = ">=", 
	[N_GT] = ">", 
	[N_NEG] = "-", 
	[N_ADD] = "+", 
	[N_SUB] = "-", 
	[N_MUL] = "*", 
	[N_DIV] = "/", 
	[N_POW] = "**", 
	[N_MOD] = "%%", 
};


static char *type_name_table[] = {
	[T_VOID] = "void",
	[T_BOOL] = "boolean",
	[T_INT] = "integer",
	[T_FLOAT] = "float",
	[T_STRING] = "String",
	[T_SET] = "Set",
	[T_R] = "Relation",
};

void print_node(FILE *f, const Ast *ast) {
	assert(ast != NULL);

	switch(ast->class) {
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_IDENTIFIER:
		fprintf(f, "%s", ast->value);
		break;
	default:
		if(node_name_table[ast->class] == NULL) {
			fprintf(f, "%s:%d:%s TODO %d", __FILE__, __LINE__, __func__, ast->class);
			return;
		}
		fprintf(f, "%s", node_name_table[ast->class]);
	}
}

void
print_node_symbolinfo(FILE *f, const Ast *ast) {
	assert(ast->symbol != NULL);

	Symbol *symbol = ast->symbol;

	print_node(f, ast);
	fprintf(f, " has symbol <%s> ", symbol->name);
	fprintf(f, "(");
	fprintf(f, "assigned=%s|", (symbol->assigned) ? "yes" : "NO!");
	fprintf(f, "read=%s", (symbol->read) ? "yes" : "NO!");
	fprintf(f, ")");
}

void
print_node_typeinfo(FILE *f, const Ast *ast) {
	print_node(f, ast);
	fprintf(f, " has type ");
	print_type(f, ast->eval_type);
	if(ast->promoted_type != T_NONE) {
		fprintf(f, "(promoted to ");
		print_type(f, ast->promoted_type);
		fprintf(f, ")");
	}
}


void
print_tree(FILE *f, const Ast *ast) {
	assert(ast != NULL);

	switch(ast->class) {
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_IDENTIFIER:
		print_node(f, ast);
		return; // no children
	default:
		;
	}

	fprintf(f, "(");
	print_node(f, ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		fprintf(f, " ");
		print_tree(f, c);
	}
	fprintf(f, ")");
}

void
print_tree_symbolinfo(FILE *f, const Ast *ast) {
	assert(ast != NULL);

	print_node_symbolinfo(f, ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		print_node_symbolinfo(f, c);
	}
}

void
print_tree_typeinfo(FILE *f, const Ast *ast) {
	assert(ast != NULL);

	print_node_typeinfo(f, ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		print_node_typeinfo(f, c);
	}
}

void
print_type(FILE *f, const Type type) {
	fprintf(f, "type(%d)", type);

	assert(type >= 0);
	assert(type < NTYPES);

	fprintf(f, " => %s", type_name_table[type]);
}
