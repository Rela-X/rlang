#include <stdlib.h>
#include <assert.h>

#include <relafix/text_io.h>

#include "print.h"
#include "types.h"
#include "utils.h"

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

void
err(const char *fmt, ...) {
	char fmt_pretty[1024];

	snprintf(fmt_pretty, sizeof(fmt_pretty), "[ERROR] %s\n",
		 fmt
	);

	va_list args;
	va_start(args, fmt);
	vdprintf(2, fmt_pretty, args);
	va_end(args);
}

void
err_at(const YYLTYPE *yyllocp, const char *fmt, ...) {
	char fmt_pretty[1024];

	snprintf(fmt_pretty, sizeof(fmt_pretty), "[ERROR@%d:%d-%d:%d] %s\n",
		yyllocp->first_line,
		yyllocp->first_column,
		yyllocp->last_line,
		yyllocp->last_column,
		fmt
	);

	va_list args;
	va_start(args, fmt);
	vdprintf(2, fmt_pretty, args);
	va_end(args);
}

void
dprint_node(int fd, const Ast *ast) {
	assert(ast != NULL);

	switch(ast->class) {
	case N_IDENTIFIER:
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
		dprintf(fd, "%s", ast->value);
		break;
	case N_SET:
		dprintf(fd, "{");
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			dprintf(fd, " ");
			dprint_node(fd, c);
		}
		dprintf(fd, " ");
		dprintf(fd, "}");
		break;
	case N_R:
		dprintf(fd, "[RELATION]");
		break;
	default:
		if(ast->class >= len(node_name_table)) {
			dprintf(fd, "%s:%d:%s TODO %d", __FILE__, __LINE__, __func__, ast->class);
			return;
		}
		dprintf(fd, "%s", node_name_table[ast->class]);
	}
}

void
dprint_node_symbolinfo(int fd, const Ast *ast) {
	assert(ast != NULL);

	dprint_node(fd, ast);

	Symbol *symbol = ast->symbol;
	assert(symbol != NULL);

	dprintf(fd, " has symbol <%s> ", symbol->name);
	dprintf(fd, "(");
	dprintf(fd, "assigned=%s|", (symbol->assigned) ? "yes" : "NO!");
	dprintf(fd, "read=%s", (symbol->read) ? "yes" : "NO!");
	dprintf(fd, ")");
}

void
dprint_node_typeinfo(int fd, const Ast *ast) {
	assert(ast != NULL);

	dprint_node(fd, ast);
	dprintf(fd, " has type ");
	dprint_type(fd, ast->eval_type);
	if(ast->promoted_type != T_NONE) {
		dprintf(fd, "(promoted to ");
		dprint_type(fd, ast->promoted_type);
		dprintf(fd, ")");
	}
}


void
dprint_tree(int fd, const Ast *ast) {
	assert(ast != NULL);

	switch(ast->class) {
	case N_IDENTIFIER:
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_SET:
	case N_R:
		dprint_node(fd, ast);
		return; // no children
	default:
		;
	}

	dprintf(fd, "(");
	dprint_node(fd, ast);
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		dprintf(fd, " ");
		dprint_tree(fd, c);
	}
	dprintf(fd, ")");
}

void
dprint_tree_symbolinfo(int fd, const Ast *ast) {
	assert(ast != NULL);

	if(ast->symbol != NULL) {
		dprint_node_symbolinfo(fd, ast);
		dprintf(fd, "\n");
	}
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		dprint_tree_symbolinfo(fd, c);
	}
}

void
dprint_tree_typeinfo(int fd, const Ast *ast) {
	assert(ast != NULL);

	dprint_node_typeinfo(fd, ast);
	dprintf(fd, "\n");
	for(Ast *c = ast->child; c != NULL; c = c->next) {
		dprint_tree_typeinfo(fd, c);
	}
}

void
dprint_type(int fd, const Type type) {
	dprintf(fd, "type(%d)", type);

	assert(type >= 0);
	assert(type < len(type_name_table));

	dprintf(fd, " => %s", type_name_table[type]);
}

void
dprint_value(int fd, const Value *value) {
	assert(value != NULL);

	dprintf(fd, "<");

	if(value->type > 0 && value->type < len(type_name_table)) {
		dprintf(fd, "%s", type_name_table[value->type]);
	} else if(value->type == T_VOID ){
		dprintf(fd, "void>");
		return;
	} else {
		dprintf(fd, "%d", value->type);
	}

	dprintf(fd, ":");

	switch(value->type) {
	case T_BOOL:
		dprintf(fd, "%s", (value->as_bool) ? "true" : "false");
		break;
	case T_INT:
		dprintf(fd, "%d", value->as_int);
		break;
	case T_FLOAT:
		dprintf(fd, "%f", value->as_float);
		break;
	case T_STRING:
		dprintf(fd, "%s", value->as_String);
		break;
	case T_SET: {
		char *str = rf_set_to_string(value->as_Set);
		dprintf(fd, "%s", str);
		rf_string_free(str);
		} break;
	case T_R: {
		char *str = rf_relation_to_string(value->as_Relation);
		dprintf(fd, "%s", str);
		rf_string_free(str);
		} break;
	default:
		dprintf(fd, "%s:%d:%s TODO ", __FILE__, __LINE__, __func__);
		dprint_type(fd, value->type);
	}
	dprintf(fd, ">");
}
