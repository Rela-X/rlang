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
	[N_RETURN] = "return",
	[N_NOT] = "!", 
	[N_EQ] = "==", 
	[N_NEQ] = "!=",
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
	assert(ast != NULL);

	print_node(f, ast);

	Symbol *symbol = ast->symbol;
	assert(symbol != NULL);

	fprintf(f, " has symbol <%s> ", symbol->name);
	fprintf(f, "(");
	fprintf(f, "assigned=%s|", (symbol->assigned) ? "yes" : "NO!");
	fprintf(f, "read=%s", (symbol->read) ? "yes" : "NO!");
	fprintf(f, ")");
}

void
print_node_typeinfo(FILE *f, const Ast *ast) {
	assert(ast != NULL);

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

	if(ast->symbol != NULL) {
		print_node_symbolinfo(f, ast);
		fprintf(f, "\n");
	}
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		print_tree_symbolinfo(f, c);
	}
}

void
print_tree_typeinfo(FILE *f, const Ast *ast) {
	assert(ast != NULL);

	print_node_typeinfo(f, ast);
	fprintf(f, "\n");
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		print_tree_typeinfo(f, c);
	}
}

void
print_type(FILE *f, const Type type) {
	fprintf(f, "type(%d)", type);

	assert(type >= 0);
	assert(type < NTYPES);

	fprintf(f, " => %s", type_name_table[type]);
}

void
print_value(FILE *f, const Value *value) {
	assert(value != NULL);

	fprintf(f, "<");

	if(value->type > 0 && value->type < NTYPES) {
		fprintf(f, "%s", type_name_table[value->type]);
	} else if(value->type == T_VOID ){
		fprintf(f, "void>");
		return;
	} else {
		fprintf(f, "%d", value->type);
	}

	fprintf(f, ":");

	switch(value->type) {
	case T_BOOL:
		fprintf(f, "%s", (value->as_bool) ? "true" : "false");
		break;
	case T_INT:
		fprintf(f, "%d", value->as_int);
		break;
	case T_FLOAT:
		fprintf(f, "%f", value->as_float);
		break;
	case T_STRING:
		fprintf(f, "%s", value->as_String);
		break;
	default:
		fprintf(f, "%s:%d:%s TODO ", __FILE__, __LINE__, __func__);
		print_type(f, value->type);
	}
	fprintf(f, ">");
}
