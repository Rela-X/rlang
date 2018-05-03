#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "types.h"

static int validate_tree(const Ast *ast);
static int validate_ifstatement(const Ast *);
static int validate_whilestatement(const Ast *);
static int validate_return(const Ast *);
static int validate_call(const Ast *);
static int validate_assignment(const Ast *);
static int validate_relational_expression(const Ast *);
static bool valid_as_type(Ast *, const Type);
static bool expr_type_ok(Ast *, const Type);

static const Type type_promotion_table[NTYPES][NTYPES] = {
                       /* void */ /* boolean */ /* integer */ /* float */ /* String */ /* Set */ /* R */
        [T_VOID]   = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
        [T_BOOL]   = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
        [T_INT]    = { T_NONE,    T_NONE,       T_NONE,       T_FLOAT,    T_NONE,      T_NONE,   T_NONE },
        [T_FLOAT]  = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
        [T_STRING] = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
        [T_SET]    = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
        [T_R]      = { T_NONE,    T_NONE,       T_NONE,       T_NONE,     T_NONE,      T_NONE,   T_NONE },
};

int
ast_validate_types(const Ast *ast) {
	printf("validating type annotations\n");
	return validate_tree(ast);
}

static
int
validate_tree(const Ast *ast) {
	switch(ast->class) {
	case N_IF:
		return validate_ifstatement(ast);
	case N_WHILE:
		return validate_whilestatement(ast);
	case N_RETURN:
		return validate_return(ast);
	case N_CALL:
		return validate_call(ast);
	case N_ASSIGNMENT:
		return validate_assignment(ast);
	case N_R:
		return validate_relational_expression(ast);
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			int err = 0;
			if((err = validate_tree(c))) return err;
		}
	}

	return 0;
}

static
int
validate_ifstatement(const Ast *if_stmt) {
	Ast *cond_expr = if_stmt->child;
//	Ast *then_stmt = if_stmt->child->next;
//	Ast *else_stmt = if_stmt->child->next->next;

	if(!expr_type_ok(cond_expr, T_BOOL)) return 1;
	return 0;
}

static
int
validate_whilestatement(const Ast *while_stmt) {
	Ast *cond_expr = while_stmt->child;
//	Ast *loop_body = while_stmt->child->next;

	if(!expr_type_ok(cond_expr, T_BOOL)) return 1;
	return 0;
}

static
int
validate_return(const Ast *return_stmt) {
	Ast *expr = return_stmt->child;

	if(!expr_type_ok(expr, return_stmt->eval_type)) return 1;
	return 0;
}

static
int
validate_call(const Ast *call) {
	Ast *id = call->child;
	Ast *cargs = call->child->next;

	Ast *carg = cargs->child;
	Symbol *farg = id->symbol->args->symbols;
	int err = 0;
	for(;carg != NULL && farg != NULL; carg = carg->next, farg = farg->next) {
		err |= !expr_type_ok(carg, farg->eval_type);
	}
	if(carg != NULL || farg != NULL) {
		print_tree(stdout, call);
		printf(" has invalid number of args\n");
		err |= 1;
	}

	return err;
}

static
int
validate_assignment(const Ast *assignment) {
	Ast *id = assignment->child;
	Ast *expr = assignment->child->next;

	if(!expr_type_ok(expr, id->eval_type)) return 1;
	return 0;
}

static
int
validate_relational_expression(const Ast *relation) {
	Ast *domain1 = relation->child;
	Ast *domain2 = relation->child->next;
//	Ast *rtable = relation->child->next->next;

	if(!expr_type_ok(domain1, T_SET)) return 1;
	if(!expr_type_ok(domain2, T_SET)) return 1;
	return 0;
}

static inline
bool
valid_as_type(Ast *expr, const Type t) {
	if(expr->eval_type == t)
		return true;
	expr->promoted_type = type_promotion_table[expr->eval_type][t];

	return expr->promoted_type == t;
}

static
bool
expr_type_ok(Ast *expr, const Type t) {
	if(!valid_as_type(expr, t)) {
		print_tree(stdout, expr);
		printf(" not a valid as ");
		print_type(stdout, t);
		printf("\n");
		return false;
	}
	return true;
}
