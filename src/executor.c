#include <stdlib.h>
#include <assert.h>

#include <math.h>
#include <string.h>

#include "ast.h"
#include "memory.h"

static void exec(const Ast *);
static void block(const Ast *);
static void ifstat(const Ast *);
static void whilestat(const Ast *);
static void declaration(const Ast *);

static Value eval(const Ast *);
static Value assignment(const Ast *);
static Value _neg(const Ast *);
static Value _not(const Ast *);
static Value _eq(const Ast *);
static Value _neq(const Ast *);
static Value _and(const Ast *);
static Value _ior(const Ast *);
static Value _lt(const Ast *);
static Value _le(const Ast *);
static Value _ge(const Ast *);
static Value _gt(const Ast *);
static Value _add(const Ast *);
static Value _sub(const Ast *);
static Value _mul(const Ast *);
static Value _div(const Ast *);
static Value _pow(const Ast *);
static Value _mod(const Ast *);
static Value identifier(const Ast *);
static Value _bool(const Ast *);
static Value _int(const Ast *);
static Value _float(const Ast *);
static Value _string(const Ast *);
static Value _set(const Ast *);
static Value _relation(const Ast *);

static MemorySpace *current_memspace = NULL;

#define EQUALS(v1, v2) (_Generic((v1, v2),   \
	bool, bool	: equals_bb(v1, v2), \
	) (v1, v2)

void
ast_execute(const Ast *ast) {
	printf("executing program\n");

	exec(ast);
}

static
void
exec(const Ast *ast) {
printf("executing "); ast_print_node(ast); printf("\n");
	switch(ast->class) {
	case N_BLOCK:
		block(ast);
		return;
	case N_IF:
		ifstat(ast);
		return;
	case N_WHILE:
		whilestat(ast);
		return;
	case N_DECLARATION:
		declaration(ast);
		return;
	case N_ASSIGNMENT:
	case N_NEG:
	case N_NOT:
	case N_EQ:
	case N_NEQ:
	case N_AND:
	case N_IOR:
	case N_XOR:
	case N_LT:
	case N_LE:
	case N_GE:
	case N_GT:
	case N_ADD:
	case N_SUB:
	case N_MUL:
	case N_DIV:
	case N_POW:
	case N_MOD:
	case N_IDENTIFIER:
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_SET:
	case N_R:
		eval(ast);
		return;
	}
printf("EXECFAIL "); ast_print_node(ast); printf("\n");
	assert(false && "should not be reached");
}

static
void
block(const Ast *block) {
	MemorySpace *new_memspace = memspace_new(current_memspace);
	current_memspace = new_memspace;

	for(Ast *c = block->child; c != NULL; c = c->next) {
		exec(c);
	}

	current_memspace = current_memspace->parent;
	memspace_free(new_memspace);
}

static
void
ifstat(const Ast *if_stmt) {
	Ast *cond_expr = if_stmt->child;
	Ast *then_stmt = if_stmt->child->next;
	Ast *else_stmt = if_stmt->child->next->next;

	if(eval(cond_expr).as_bool == true) {
		exec(then_stmt);
	} else {
		exec(else_stmt);
	}
}

static
void
whilestat(const Ast *while_stmt) {
	Ast *cond_expr = while_stmt->child;
	Ast *loop_body = while_stmt->child->next;

	while(eval(cond_expr).as_bool == true) {
		exec(loop_body);
	}
}

static
void 
declaration(const Ast *declaration) {
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;
	Ast *expr = declaration->child->next->next;

	Memory *m = mem_new(id->symbol);
	memspace_store(current_memspace, m);

	if(expr != NULL) {
		m->value = eval(expr);
	}
}

static
Value
eval(const Ast *expr) {
printf("evaluating "); ast_print_tree(expr); printf("\n");
	switch(expr->class) {
	case N_ASSIGNMENT:
		return assignment(expr);
	case N_NEG:
		return _neg(expr);
	case N_NOT:
		return _not(expr);
	case N_EQ:
		return _eq(expr);
	case N_NEQ:
		return _neq(expr);
	case N_AND:
		return _and(expr);
	case N_IOR:
		return _ior(expr);
	case N_XOR:
		return _neq(expr); // alias
	case N_LT:
		return _lt(expr);
	case N_LE:
		return _le(expr);
	case N_GE:
		return _ge(expr);
	case N_GT:
		return _gt(expr);
	case N_ADD:
		return _add(expr);
	case N_SUB:
		return _sub(expr);
	case N_MUL:
		return _mul(expr);
	case N_DIV:
		return _div(expr);
	case N_POW:
		return _pow(expr);
	case N_MOD:
		return _mod(expr);
	case N_IDENTIFIER:
		return identifier(expr);
	case N_BOOLEAN:
		return _bool(expr);
	case N_INTEGER:
		return _int(expr);
	case N_FLOAT:
		return _float(expr);
	case N_STRING:
		return _string(expr);
	/*
	case N_SET:
	case N_R:
	*/
	}
printf("EVALFAIL "); ast_print_node(expr); printf("\n");
	assert(false && "should not be reached");
}

static
Value
assignment(const Ast *ast) {
	Ast *id = ast->child;
	Ast *expr = ast->child->next;

	Memory *m = memspace_load(current_memspace, id->symbol);
	m->value = eval(expr);
}

static
Value
_neg(const Ast *expr) {
	Ast *op1 = expr->child;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = - eval(op1).as_int;
		break;
	case T_FLOAT:
		r.as_float = - eval(op1).as_float;
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_not(const Ast *expr) {
	Ast *op1 = expr->child;
	Value r;

	assert(expr->eval_type == T_BOOL);

	r.as_bool = ! eval(op1).as_bool;

	return r;
}

static
Value
_eq(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	Type t = (op1->promoted_type != T_NONE) ? op1->promoted_type : op1->eval_type;
	switch(t) {
	case T_BOOL:
		r.as_bool = eval(op1).as_bool == eval(op2).as_bool;
		break;
	case T_INT:
		r.as_bool = eval(op1).as_int == eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_bool = eval(op1).as_float == eval(op2).as_float;
		break;
	case T_STRING:
		r.as_bool = strcmp(eval(op1).as_str, eval(op2).as_str) == 0;
		break;
	case T_SET:
		// TODO
		break;
	case T_R:
		// TODO
		break;
	}

	return r;
}


static
Value
_neq(const Ast *expr) {
//	Ast *op1 = expr->child;
//	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = ! _eq(expr).as_bool;

	return r;
}


static
Value
_and(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_bool && eval(op2).as_bool;

	return r;
}


static
Value
_ior(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_bool || eval(op2).as_bool;

	return r;
}


static
Value
_lt(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_float < eval(op2).as_float;

	return r;
}

static
Value
_le(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_float <= eval(op2).as_float;

	return r;
}

static
Value
_ge(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_float >= eval(op2).as_float;

	return r;
}

static
Value
_gt(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;

	r.as_bool = eval(op1).as_float > eval(op2).as_float;

	return r;
}

static
Value
_add(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = eval(op1).as_int + eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_float = eval(op1).as_float + eval(op2).as_float;
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_sub(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = eval(op1).as_int - eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_float = eval(op1).as_float - eval(op2).as_float;
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_mul(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = eval(op1).as_int * eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_float = eval(op1).as_float * eval(op2).as_float;
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_div(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = eval(op1).as_int / eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_float = eval(op1).as_float / eval(op2).as_float;
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_pow(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = (int)pow(eval(op1).as_float, eval(op2).as_float);
		break;
	case T_FLOAT:
		r.as_float = pow(eval(op1).as_float, eval(op2).as_float);
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
_mod(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;
	Value r;
	switch(expr->eval_type) {
	case T_INT:
		r.as_int = eval(op1).as_int % eval(op2).as_int;
		break;
	case T_FLOAT:
		r.as_float = fmod(eval(op1).as_float, eval(op2).as_float);
		break;
	default:
		assert(false);
	}

	return r;
}

static
Value
identifier(const Ast *expr) {
	Memory *m = memspace_load(current_memspace, expr->symbol);
	assert(m != NULL);
	Value r = m->value;

	return r;
}

static
Value
_bool(const Ast *expr) {
	Value r;
	char *s = expr->value;
	if(s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e' && s[4] == '\0') {
		r.as_bool = true;
	} else if(s[0] == 'f' && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' && s[5] == '\0') {
		r.as_bool = false;
	} else {
		assert(false);
	}

	return r;
}

static
Value
_int(const Ast *expr) {
	Value r;
	r.as_int = atoi(expr->value);

	return r;
}

static
Value
_float(const Ast *expr) {
	Value r;
	r.as_float = atof(expr->value);

	return r;
}

static
Value
_string(const Ast *expr) {
	Value r;
	r.as_str = expr->value;

	return r;
}

static
Value
_set(const Ast *expr) {
}

static
Value
_relation(const Ast *expr) {
}

