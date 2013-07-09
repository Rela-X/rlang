#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "types.h"

static RLangType get_type(Ast *);
static RLangType get_type_identifier(const Ast *);
static RLangType get_type_declaration(const Ast *);
static RLangType get_type_binary_op(const Ast *);
static RLangType result_type(const RLangType type_table[NTYPES][NTYPES], Ast *, Ast *);

static const RLangType arithmetic_result_type_table[NTYPES][NTYPES] = {
                        /* void */ /* boolean */ /* integer */ /* float */ /* String */ /* R */
        [rl_VOID]   = { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
        [rl_BOOL]   = { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
        [rl_INT]    = { rl_VOID,   rl_VOID,      rl_INT,       rl_FLOAT,   rl_VOID,     rl_VOID },
        [rl_FLOAT]  = { rl_VOID,   rl_VOID,      rl_FLOAT,     rl_FLOAT,   rl_VOID,     rl_VOID },
        [rl_STRING] = { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
        [rl_R]      = { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
};

static const RLangType type_promotion_table[NTYPES][NTYPES] = {
                        /* void */ /* boolean */ /* integer */ /* float */ /* String */ /* R */
        [rl_VOID]   = { rl_NONE,   rl_NONE,      rl_NONE,      rl_NONE,    rl_NONE,     rl_NONE },
        [rl_BOOL]   = { rl_NONE,   rl_NONE,      rl_NONE,      rl_NONE,    rl_NONE,     rl_NONE },
        [rl_INT]    = { rl_NONE,   rl_NONE,      rl_NONE,      rl_FLOAT,   rl_NONE,     rl_NONE },
        [rl_FLOAT]  = { rl_NONE,   rl_NONE,      rl_NONE,      rl_NONE,    rl_NONE,     rl_NONE },
        [rl_STRING] = { rl_NONE,   rl_NONE,      rl_NONE,      rl_NONE,    rl_NONE,     rl_NONE },
        [rl_R]      = { rl_NONE,   rl_NONE,      rl_NONE,      rl_NONE,    rl_NONE,     rl_NONE },
};

void
check_types(Ast *ast) {
	ast->eval_type = get_type(ast);

	assert(ast->eval_type != rl_NONE);
ast_print_node(ast); printf(" evaluates to "); print_type(ast->eval_type);
if(ast->promoted_type != rl_NONE) { printf(" (promoted to "); print_type(ast->promoted_type); printf(")"); }
printf("\n");

	/* we have gathered the type information for this tree already */
	if(ast->eval_type != rl_VOID)
		return;

	for(Ast *c = ast->child; c != NULL; c = c->next) {
		check_types(c);
	}
}

static
RLangType
get_type(Ast *ast) {
	switch(ast->type) {
	case NT_BOOLEAN:
		return rl_BOOL;
	case NT_INTEGER:
		return rl_INT;
	case NT_FLOAT:
		return rl_FLOAT;
	case NT_STRING:
		return rl_STRING;
	case NT_R:
		return rl_R;
	case NT_IDENTIFIER:
		return get_type_identifier(ast);
	case NT_DECLARESTMT:
		return get_type_declaration(ast);
	case NT_NOT:
	case NT_EQ: case NT_AND: case NT_IOR: case NT_XOR:
	case NT_LT: case NT_LE: case NT_GE: case NT_GT:
		return rl_BOOL;
	case NT_NEG:
		return get_type(ast->child);
	case NT_ADD: case NT_SUB: case NT_MUL: case NT_DIV: case NT_POW: case NT_MOD:
		return get_type_binary_op(ast);
	default:
		return rl_VOID;
	}
}

static
RLangType
get_type_identifier(const Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);

	return sy->eval_type;
}

static
RLangType
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

	return rl_VOID;
}

static
RLangType
get_type_binary_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;
	check_types(left);
	check_types(right);
	return result_type(arithmetic_result_type_table, left, right);
}

static
RLangType
result_type(const RLangType type_table[NTYPES][NTYPES], Ast *a, Ast *b) {
	RLangType result = type_table[a->eval_type][b->eval_type];
	a->promoted_type = type_promotion_table[a->eval_type][result];
	b->promoted_type = type_promotion_table[b->eval_type][result];

	return result;
}

