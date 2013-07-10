#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "types.h"

static void annotate_tree(Ast *);
static Type identifier(const Ast *);
static Type declaration(const Ast *);
static Type arithmetic_op(const Ast *);
static Type relational_op(const Ast *);
static Type op_type(const Type type_table[NTYPES][NTYPES], const Ast *, const Ast *);

static const Type arithmetic_result_type_table[NTYPES][NTYPES] = {
                       /* void */  /* boolean */ /* integer */ /* float */ /* String */ /* Set */ /* R */
        [T_VOID]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_BOOL]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_INT]    = { T_VOID,     T_VOID,       T_INT,        T_FLOAT,    T_VOID,      T_VOID,   T_VOID },
        [T_FLOAT]  = { T_VOID,     T_VOID,       T_FLOAT,      T_FLOAT,    T_VOID,      T_VOID,   T_VOID },
        [T_STRING] = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_SET]    = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_R]      = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
};

static const Type relational_result_type_table[NTYPES][NTYPES] = {
                       /* void */  /* boolean */ /* integer */ /* float */ /* String */ /* Set */ /* R */
        [T_VOID]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_BOOL]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_INT]    = { T_VOID,     T_VOID,       T_BOOL,       T_BOOL,     T_VOID,      T_VOID,   T_VOID },
        [T_FLOAT]  = { T_VOID,     T_VOID,       T_BOOL,       T_BOOL,     T_VOID,      T_VOID,   T_VOID },
        [T_STRING] = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_SET]    = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_R]      = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
};

void
ast_annotate_types(Ast *ast) {
	printf("setting type annotations\n");

	annotate_tree(ast);
}

static
void
annotate_tree(Ast *ast) {
	switch(ast->class) {
	case N_BOOLEAN:
		ast->eval_type = T_BOOL;
		break;
	case N_INTEGER:
		ast->eval_type = T_INT;
		break;
	case N_FLOAT:
		ast->eval_type = T_FLOAT;
		break;
	case N_STRING:
		ast->eval_type = T_STRING;
		break;
	case N_SET:
		ast->eval_type = T_SET;
		break;
	case N_R:
		ast->eval_type = T_R;
		break;
	case N_IDENTIFIER:
		ast->eval_type = identifier(ast);
		break;
	case N_DECLARESTMT:
		ast->eval_type = declaration(ast);
		break;
	case N_NOT:
		annotate_tree(ast->child);
		ast->eval_type = ast->child->eval_type;
		break;
	case N_EQ: case N_AND: case N_IOR: case N_XOR:
		ast->eval_type = T_BOOL; // TODO
		break;
	case N_LT: case N_LE: case N_GE: case N_GT:
		ast->eval_type = relational_op(ast);
		break;
	case N_NEG:
		annotate_tree(ast->child);
		ast->eval_type = ast->child->eval_type;
		break;
	case N_ADD: case N_SUB: case N_MUL: case N_DIV: case N_POW: case N_MOD:
		ast->eval_type = arithmetic_op(ast);
		break;
	default:
		ast->eval_type = T_VOID;
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			annotate_tree(c);
		}
	}
ast_print_node(ast); printf(" has type "); print_type(ast->eval_type); printf("\n");
}

static
Type
identifier(const Ast *id) {
	assert(id->symbol != NULL);
	return id->symbol->eval_type;
}

static
Type
declaration(const Ast *declaration) {
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;
	Ast *expr = declaration->child->next->next;

	/* set the type for the SYMBOL, not just for the id-node */
	id->symbol->eval_type = type->symbol->eval_type;

	annotate_tree(type);
	annotate_tree(id);
	if(expr != NULL) {
		annotate_tree(expr);
	}

	return T_VOID;
}

static
Type
arithmetic_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	annotate_tree(left);
	annotate_tree(right);

	Type t = op_type(arithmetic_result_type_table, left, right);

	return t;
}

static
Type
relational_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	annotate_tree(left);
	annotate_tree(right);

	Type t = op_type(relational_result_type_table, left, right);

	return t;
}

static inline
Type
op_type(const Type type_table[NTYPES][NTYPES], const Ast *a, const Ast *b) {
	return type_table[a->eval_type][b->eval_type];
}

