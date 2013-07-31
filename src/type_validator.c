#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "print.h"
#include "types.h"

static void validate_tree(const Ast *ast);
static void validate_ifstatement(const Ast *);
static void validate_assignment(const Ast *);
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
validate_assignment(const Ast *assignment) {
	Ast *id = assignment->child;
	Ast *expr = assignment->child->next;

	if(!valid_as_type(expr, id->eval_type)) {
		print_tree(stdout, expr);
		printf(" not a valid as ");
		print_type(stdout, id->eval_type);
		printf("\n");
	}
}

static
void
validate_ifstatement(const Ast *if_stmt) {
	Ast *cond_expr = if_stmt->child;
//	Ast *then_stmt = if_stmt->child->next;
//	Ast *else_stmt = if_stmt->child->next->next;

	if(!valid_as_type(cond_expr, T_BOOL)) {
		print_tree(stdout, cond_expr);
		printf(" not a valid as ");
		print_type(stdout, T_BOOL);
		printf("\n");
	}
}

static inline
bool
valid_as_type(Ast *ast, const Type t) {
	if(ast->eval_type == t)
		return true;
	ast->promoted_type = type_promotion_table[ast->eval_type][t];

	return ast->promoted_type == t;
}

