#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "types.h"

static void validate_tree(const Ast *ast);
static void validate_ifstatement(const Ast *);
static void validate_whilestatement(const Ast *);
static void validate_assignment(const Ast *);
static void validate_call(const Ast *);
static void validate_return(const Ast *);
static bool valid_as_type(Ast *, Type);

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

#define expr_check_type(expr, type) do {	\
	if(!valid_as_type(expr, type)) {	\
		print_tree(stdout, expr);	\
		printf(" not a valid as ");	\
		print_type(stdout, type);	\
		printf("\n");			\
	} \
} while(0)

void
ast_validate_types(const Ast *ast) {
	printf("validating type annotations\n");
	validate_tree(ast);
}

static
void
validate_tree(const Ast *ast) {
	switch(ast->class) {
	case N_IF:
		validate_ifstatement(ast);
		break;
	case N_WHILE:
		validate_whilestatement(ast);
		break;
	case N_RETURN:
		validate_return(ast);
		break;
	case N_CALL:
		validate_call(ast);
		break;
	case N_ASSIGNMENT:
		validate_assignment(ast);
		break;
	default:
		for(Ast *c = ast->child; c != NULL; c = c->next) {
			validate_tree(c);
		}
	}
}

static
void
validate_ifstatement(const Ast *if_stmt) {
	Ast *cond_expr = if_stmt->child;
//	Ast *then_stmt = if_stmt->child->next;
//	Ast *else_stmt = if_stmt->child->next->next;

	expr_check_type(cond_expr, T_BOOL);
}

static
void
validate_whilestatement(const Ast *while_stmt) {
	Ast *cond_expr = while_stmt->child;
//	Ast *loop_body = while_stmt->child->next;

	expr_check_type(cond_expr, T_BOOL);
}

static
void
validate_return(const Ast *return_stmt) {
	Ast *expr = return_stmt->child;

	expr_check_type(expr, return_stmt->eval_type);
}

static
void
validate_call(const Ast *call) {
	Ast *id = call->child;
	Ast *cargs = call->child->next;

	Ast *carg = cargs->child;
	Symbol *farg = id->symbol->args->symbols;
	for(;carg != NULL && farg != NULL; carg = carg->next, farg = farg->next) {
		expr_check_type(carg, farg->eval_type);
	}
	if(carg != NULL || farg != NULL) {
		print_tree(stdout, call);
		printf(" has invalid number of args\n");
	}
}

static
void
validate_assignment(const Ast *assignment) {
	Ast *id = assignment->child;
	Ast *expr = assignment->child->next;

	expr_check_type(expr, id->eval_type);
}

static inline
bool
valid_as_type(Ast *ast, const Type t) {
	if(ast->eval_type == t)
		return true;
	ast->promoted_type = type_promotion_table[ast->eval_type][t];

	return ast->promoted_type == t;
}

