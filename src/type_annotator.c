#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "types.h"

static Type annotate_tree(Ast *);
static Type vardecl(const Ast *);
static Type function(const Ast *);
static Type return_stmt(const Ast *);
static Type call(const Ast *);
static Type unary_op(const Ast *);
static Type equality_op(const Ast *);
static Type boolean_op(const Ast *);
static Type arithmetic_op(const Ast *);
static Type relational_op(const Ast *);
static Type identifier(const Ast *);
static Type set_symbol_type(Ast *, Ast *);
static Type get_symbol_type(const Ast *);
static Type op_type(const Type type_table[NTYPES][NTYPES], const Ast *, const Ast *);

static const Type equality_result_type_table[NTYPES][NTYPES] = {
                       /* void */  /* boolean */ /* integer */ /* float */ /* String */ /* Set */ /* R */
        [T_VOID]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_BOOL]   = { T_VOID,     T_BOOL,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_INT]    = { T_VOID,     T_VOID,       T_BOOL,       T_BOOL,     T_VOID,      T_VOID,   T_VOID },
        [T_FLOAT]  = { T_VOID,     T_VOID,       T_BOOL,       T_BOOL,     T_VOID,      T_VOID,   T_VOID },
        [T_STRING] = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_BOOL,      T_VOID,   T_VOID },
        [T_SET]    = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_BOOL,   T_VOID },
        [T_R]      = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_BOOL },
};

static const Type boolean_result_type_table[NTYPES][NTYPES] = {
                       /* void */  /* boolean */ /* integer */ /* float */ /* String */ /* Set */ /* R */
        [T_VOID]   = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_BOOL]   = { T_VOID,     T_BOOL,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_INT]    = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_FLOAT]  = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_STRING] = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_SET]    = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
        [T_R]      = { T_VOID,     T_VOID,       T_VOID,       T_VOID,     T_VOID,      T_VOID,   T_VOID },
};

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

	ast->eval_type = annotate_tree(ast);
}

static
Type
annotate_tree(Ast *ast) {
	switch(ast->class) {
	case N_DECLARATION:
		return vardecl(ast);
	case N_FUNCTION:
		return function(ast);
	case N_RETURN:
		return return_stmt(ast);
	case N_CALL:
		return call(ast);
	case N_NEG:
	case N_NOT:
		return unary_op(ast);
	case N_EQ:
	case N_NEQ:
		return equality_op(ast);
	case N_AND:
	case N_IOR:
	case N_XOR:
		return boolean_op(ast);
	case N_LT:
	case N_LE:
	case N_GE:
	case N_GT:
		return relational_op(ast);
	case N_ADD:
	case N_SUB:
	case N_MUL:
	case N_DIV:
	case N_POW:
	case N_MOD:
		return arithmetic_op(ast);
	case N_IDENTIFIER:
		return identifier(ast);
	case N_BOOLEAN:
		return T_BOOL;
	case N_INTEGER:
		return T_INT;
	case N_FLOAT:
		return T_FLOAT;
	case N_STRING:
		return T_STRING;
	case N_SET:
		return T_SET;
	case N_R:
		return T_R;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			c->eval_type = annotate_tree(c);
		}
		return T_VOID;
	}
}

static
Type
vardecl(const Ast *vardecl) {
	Ast *type = vardecl->child;
	Ast *id = vardecl->child->next;

	set_symbol_type(type, id);

	type->eval_type = annotate_tree(type);
	id->eval_type = annotate_tree(id);

	return T_VOID;
}

static
Type
function(const Ast *function) {
	Ast *type = function->child;
	Ast *id = function->child->next;
	Ast *args = function->child->next->next;
	Ast *block = function->child->next->next->next;

	set_symbol_type(type, id);

	type->eval_type = annotate_tree(type);
	id->eval_type = annotate_tree(id);
	args->eval_type = annotate_tree(args);
	block->eval_type = annotate_tree(block);

	return T_VOID;
}

static
Type
return_stmt(const Ast *return_stmt) {
	Ast *expr = return_stmt->child;

	if(expr != NULL) {
		expr->eval_type = annotate_tree(expr);
	}

	Type t = get_symbol_type(return_stmt);

	return t;
}

static
Type
call(const Ast *call) {
	Ast *id = call->child;
	Ast *args = call->child->next;

	id->eval_type = annotate_tree(id);
	args->eval_type = annotate_tree(args);

	Type t = get_symbol_type(id);

	return t;
}

static
Type
unary_op(const Ast *op) {
	op->child->eval_type = annotate_tree(op->child);

	return op->child->eval_type;
}

static
Type
equality_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	left->eval_type = annotate_tree(left);
	right->eval_type = annotate_tree(right);

	Type t = op_type(equality_result_type_table, left, right);

	return t;
}

static
Type
boolean_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	left->eval_type = annotate_tree(left);
	right->eval_type = annotate_tree(right);

	Type t = op_type(boolean_result_type_table, left, right);

	return t;
}

static
Type
arithmetic_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	left->eval_type = annotate_tree(left);
	right->eval_type = annotate_tree(right);

	Type t = op_type(arithmetic_result_type_table, left, right);

	return t;
}

static
Type
relational_op(const Ast *op) {
	Ast *left = op->child;
	Ast *right = op->child->next;

	right->eval_type = annotate_tree(left);
	left->eval_type = annotate_tree(right);

	Type t = op_type(relational_result_type_table, left, right);

	return t;
}

static
Type
identifier(const Ast *id) {
	Type t = get_symbol_type(id);

	return t;
}

static
Type
set_symbol_type(Ast *type, Ast *id) {
	assert(type->symbol != NULL);
	assert(id->symbol != NULL);

	/* set the type for the SYMBOL, not just for the id-node */
	id->symbol->eval_type = type->symbol->eval_type;

	return id->symbol->eval_type;
}

static
Type
get_symbol_type(const Ast *ast) {
	assert(ast->symbol != NULL);

	return ast->symbol->eval_type;
}

static inline
Type
op_type(const Type type_table[NTYPES][NTYPES], const Ast *a, const Ast *b) {
	return type_table[a->eval_type][b->eval_type];
}

