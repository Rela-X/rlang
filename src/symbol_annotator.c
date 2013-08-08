#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "scope.h"

static void init_global_scope(Scope *);
static void walk_tree(Ast *);
static void function(Ast *, Ast *, Ast *, Ast *);
static void annotate_return_stmts(Ast *, Symbol *);
static void declaration(Ast *, Ast *);
static void assignment(Ast *, Ast *);
static void identifier(Ast *);
static void annotate_type(Ast *);
static void annotate_identifier(Ast *);
static void define_symbol(const Ast *, SymbolClass);
static inline Symbol *resolve_symbol(const Ast *);
static inline Symbol *resolve_symbol_recursive(const Ast *);

void
ast_annotate_symbols(Ast *ast) {
	printf("setting and validating symbol annotations\n");

	init_global_scope(ast->scope);

	walk_tree(ast);
}


static
void
init_global_scope(Scope *global_scope) {
#define def_type(name, type) do {               \
	Symbol *sy = symbol_new(S_TYPE, name);  \
	sy->eval_type = type;                   \
	scope_define(global_scope, sy);         \
	} while(0)

	def_type("void", T_VOID);
	def_type("bool", T_BOOL);
	def_type("int", T_INT);
	def_type("float", T_FLOAT);
	def_type("String", T_STRING);
	def_type("Set", T_SET);
	def_type("R", T_R);
#undef def_type
}

static
void
walk_tree(Ast *ast) {
	assert(ast->scope != NULL);

	switch(ast->class) {
	case N_FUNCTION:
		function(ast->child, ast->child->next, ast->child->next->next, ast->child->next->next->next);
		break;
	case N_DECLARATION:
		declaration(ast->child, ast->child->next);
		break;
	case N_ASSIGNMENT:
		assignment(ast->child, ast->child->next);
		break;
	case N_IDENTIFIER:
		identifier(ast);
		assert(ast->symbol != NULL);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) { 
			walk_tree(c);
		}
	}
}

static
void
function(Ast *type, Ast *id, Ast *args, Ast *block) {
	annotate_type(type);
	define_symbol(id, S_FUNCTION);
	annotate_identifier(id);

	id->symbol->args = args->scope;
	id->symbol->code = block;

	annotate_return_stmts(block, id->symbol);

	walk_tree(args);
	walk_tree(block);
}

static
void
annotate_return_stmts(Ast *ast, Symbol *sy) {
	switch(ast->class) {
	case N_FUNCTION:
		// ignore nested function-blocks
		break;
	case N_RETURN:
		ast->symbol = sy;
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) { 
			annotate_return_stmts(c, sy);
		}
	}
}

static
void
declaration(Ast *type, Ast *id) {
	annotate_type(type);
	define_symbol(id, S_VARIABLE);
	annotate_identifier(id);
}

static
void
assignment(Ast *id, Ast *expr) {
	/* check expression first */
	walk_tree(expr);

	annotate_identifier(id);
	assert(id->symbol != NULL);
	id->symbol->assigned = true;
}

static
void
identifier(Ast *id) {
	annotate_identifier(id);

	if(id->symbol->class == S_VARIABLE && !id->symbol->assigned) {
		printf("use of unassigned identifier <%s>!\n", id->value);
	}
}

static
void
annotate_type(Ast *type) {
	type->symbol = resolve_symbol_recursive(type);
	if(type->symbol == NULL || type->symbol->class != S_TYPE) {
		printf("<%s> is not a valid type!\n", type->value);
		assert(false);
	}
}

static
void
annotate_identifier(Ast *id) {
	id->symbol = resolve_symbol_recursive(id);
	if(id->symbol == NULL) {
		printf("<%s> not found!\n", id->value);
		assert(false);
	}
}

static
void
define_symbol(const Ast *id, SymbolClass class) {
	Symbol *sy;
	/* check if identifier is a reserved word */
	sy = resolve_symbol_recursive(id);
	if(sy != NULL && sy->class == S_TYPE) {
		printf("<%s> is not a valid name!\n", id->value);
		assert(false);
	}
	/* check if identifier is already defined in the current (!) scope */
	sy = resolve_symbol(id);
	if(sy != NULL) {
		printf("<%s> already defined!\n", id->value);
		assert(false);
	}

	/* define symbol */
	scope_define(id->scope, symbol_new(class, id->value));
}

static inline
Symbol *
resolve_symbol(const Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);
	return sy;
}

static inline
Symbol *
resolve_symbol_recursive(const Ast *id) {
	Symbol *sy = scope_resolve_recursive(id->scope, id->value);
	return sy;
}
