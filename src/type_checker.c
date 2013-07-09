#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "types.h"

static Type get_type(Ast *);
static Type get_type_identifier(const Ast *);
static Type get_type_declaration(const Ast *);
static Type get_type_binary_op(const Ast *);
static Type result_type(const RLangType type_table[NTYPES][NTYPES], Ast *, Ast *);

static const Type arithmetic_result_type_table[NTYPES][NTYPES] = {
                       /* void */  /* boolean */ /* integer */ /* float */ /* String */ /* R */
        [T_VOID]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID },
        [T_BOOL]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID },
        [T_INT]    = { T_VOID,     T_VOID,       T_INT,        T_FLOAT,    T_VOID,      T_VOID },
        [T_FLOAT]  = { T_VOID,     T_VOID,       T_FLOAT,      T_FLOAT,    T_VOID,      T_VOID },
        [T_STRING] = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID },
        [T_R]      = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID },
};

static const Type type_promotion_table[NTYPES][NTYPES] = {
                       /* void */ /* boolean */ /* integer */ /* float */ /* String */ /* R */
        [T_VOID]   = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE },
        [T_BOOL]   = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE },
        [T_INT]    = { T_NONE,    T_NONE,       T_NONE,       T_FLOAT,    T_NONE,      T_NONE },
        [T_FLOAT]  = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE },
        [T_STRING] = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE },
        [T_R]      = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE },
};

void
check_types(Ast *ast) {
	ast->eval_type = get_type(ast);

	assert(ast->eval_type != T_NONE);
ast_print_node(ast); printf(" evaluates to "); print_type(ast->eval_type);
if(ast->promoted_type != T_NONE) { printf(" (promoted to "); print_type(ast->promoted_type); printf(")"); }
printf("\n");

	/* we have gathered the type information for this tree already */
	if(ast->eval_type != T_VOID)
		return;

	for(Ast *c = ast->child; c != NULL; c = c->next) {
		check_types(c);
	}
}

static
Type
get_type(Ast *ast) {
	switch(ast->class) {
	case N_BOOLEAN:
		return T_BOOL;
	case N_INTEGER:
		return T_INT;
	case N_FLOAT:
		return T_FLOAT;
	case N_STRING:
		return T_STRING;
	case N_R:
		return T_R;
	case N_IDENTIFIER:
		return get_type_identifier(ast);
	case N_DECLARESTMT:
		return get_type_declaration(ast);
	case N_NOT:
	case N_EQ: case N_AND: case N_IOR: case N_XOR:
	case N_LT: case N_LE: case N_GE: case N_GT:
		return T_BOOL;
	case N_NEG:
		return get_type(ast->child);
	case N_ADD: case N_SUB: case N_MUL: case N_DIV: case N_POW: case N_MOD:
		return get_type_binary_op(ast);
	default:
		return T_VOID;
	}
}

static
Type
get_type_identifier(const Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);

	return sy->eval_type;
}

static
Type
get_type_declaration(const Ast *declaration) {
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;
	Ast *expr = declaration->child->next->next;

	/* set the type for the SYMBOL, not just for the id-node */
	id->symbol->eval_type = get_type_identifier(type);

	if(expr != NULL) {
		check_types(expr);
		expr->promoted_type = type_promotion_table[expr->eval_type][id->symbol->eval_type];
	}

	return T_VOID;
}

static
Type
get_type_binary_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;
	check_types(left);
	check_types(right);
	return result_type(arithmetic_result_type_table, left, right);
}

static
Type
result_type(const Type type_table[NTYPES][NTYPES], Ast *a, Ast *b) {
	Type result = type_table[a->eval_type][b->eval_type];
	a->promoted_type = type_promotion_table[a->eval_type][result];
	b->promoted_type = type_promotion_table[b->eval_type][result];

	return result;
}

