#include <stdlib.h>
#include <assert.h>

#include <math.h>
#include <string.h>

#include "ast.h"
#include "print.h"
#include "memory.h"
#include "value.h"

static void exec(const Ast *);
static void block(const Ast *);
static void ifstat(const Ast *);
static void whilestat(const Ast *);
static void declaration(const Ast *);
static void function(const Ast *);
static void returnstat(const Ast *);

static Value * eval(const Ast *);
static Value * call(const Ast *);
static Value * assignment(const Ast *);
static Value * identifier(const Ast *);
static Value * _neg(const Ast *);
static Value * _not(const Ast *);
static Value * _eq(const Ast *);
static Value * _neq(const Ast *);
static Value * _and(const Ast *);
static Value * _ior(const Ast *);
static Value * _lt(const Ast *);
static Value * _le(const Ast *);
static Value * _ge(const Ast *);
static Value * _gt(const Ast *);
static Value * _add(const Ast *);
static Value * _sub(const Ast *);
static Value * _mul(const Ast *);
static Value * _div(const Ast *);
static Value * _pow(const Ast *);
static Value * _mod(const Ast *);
static Value * _bool(const Ast *);
static Value * _int(const Ast *);
static Value * _float(const Ast *);
static Value * _string(const Ast *);
static Value * _set(const Ast *);
static Value * _relation(const Ast *);

static MemorySpace *current_memspace = NULL;
static Value *callstack_returnvalue = NULL;

#define NATIVE_NUMBER(v) ((v->type == T_INT) ? v->as_int : v->as_float)

void
ast_execute(const Ast *ast) {
	printf("executing program\n");

	exec(ast);
}

static
void
exec(const Ast *ast) {
printf("executing "); pn(ast);
	Value *v;
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
	case N_FUNCTION:
		function(ast);
		return;
	case N_RETURN:
		returnstat(ast);
		return;
	case N_CALL:
	case N_ASSIGNMENT:
	case N_IDENTIFIER:
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
	case N_BOOLEAN:
	case N_INTEGER:
	case N_FLOAT:
	case N_STRING:
	case N_SET:
	case N_R:
		v = eval(ast);
print_tree(stdout, ast); printf(" evaluates to "); if(v != NULL) { pv(v); value_free(v); }
		if(v != NULL) {
//			value_free(v);
		} else {
			printf("v=null: "); pn(ast);
		}
		return;
	}
printf("EXECFAIL %d ", ast->class); pn(ast);
	assert(false && "should not be reached");
}

static
void
block(const Ast *block) {
	MemorySpace *old_memspace = current_memspace;
	current_memspace = memspace_new(current_memspace);

	for(Ast *c = block->child; c != NULL && callstack_returnvalue == NULL; c = c->next) {
		exec(c);
	}

	memspace_free(current_memspace);
	current_memspace = old_memspace;
}

static
void
ifstat(const Ast *if_stmt) {
	Ast *cond_expr = if_stmt->child;
	Ast *then_stmt = if_stmt->child->next;
	Ast *else_stmt = if_stmt->child->next->next;

	Value *cond;
	if((cond = eval(cond_expr))->as_bool) {
		exec(then_stmt);
	} else {
		exec(else_stmt);
	}
	value_free(cond);
}

static
void
whilestat(const Ast *while_stmt) {
	Ast *cond_expr = while_stmt->child;
	Ast *loop_body = while_stmt->child->next;

	Value *cond;
	for(cond = eval(cond_expr); cond->as_bool; value_free(cond), cond = eval(cond_expr)) {
		exec(loop_body);
	}
	value_free(cond);
}

static
void
declaration(const Ast *declaration) {
	//Ast *type = declaration->child;
	Ast *id = declaration->child->next;

	Memory *m = mem_new(id->symbol);
	memspace_store(current_memspace, m);
}

static
void
function(const Ast *function) {
	// TODO
}

static
void
returnstat(const Ast *return_stmt) {
	Ast *expr = return_stmt->child;

	callstack_returnvalue = eval(expr);
}

static
Value *
eval(const Ast *expr) {
	switch(expr->class) {
	case N_CALL:
		return call(expr);
	case N_ASSIGNMENT:
		return assignment(expr);
	case N_IDENTIFIER:
		return identifier(expr);
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
printf("EVALFAIL %d ", expr->class); pn(expr);
	assert(false && "should not be reached");
}

static
Value *
call(const Ast *expr) {
	Ast *id = expr->child;
	Ast *cargs = expr->child->next;
	Value *rval;

	MemorySpace *global_memspace = current_memspace;
	while(global_memspace->parent != NULL)
		global_memspace = global_memspace->parent;

	MemorySpace *fn_memspace = memspace_new(global_memspace);

	Ast *carg = cargs->child;
	Symbol *farg = id->symbol->args->symbols;
	for(; carg != NULL && farg != NULL; carg = carg->next, farg = farg->next) {
		Memory *m = mem_new(farg);
		m->value = eval(carg);
		memspace_store(fn_memspace, m);
	}
	MemorySpace *old_memspace = current_memspace;
	current_memspace = fn_memspace;

	exec(id->symbol->code);

	memspace_free(current_memspace);
	current_memspace = old_memspace;

	rval = callstack_returnvalue;
	callstack_returnvalue = NULL;

	return rval;
}

static
Value *
assignment(const Ast *ast) {
	Ast *id = ast->child;
	Ast *expr = ast->child->next;

	Memory *m = memspace_load(current_memspace, id->symbol);
	assert(m != NULL);
	assert(m->value != NULL);

	Value *rval = eval(expr);

	value_copy(m->value, rval);

	return rval;
}

static
Value *
identifier(const Ast *expr) {
	Memory *m = memspace_load(current_memspace, expr->symbol);
	assert(m != NULL);
	assert(m->value != NULL);

	Value *rval = value_new();

	value_copy(rval, m->value);

	return rval;
}

static
Value *
_neg(const Ast *expr) {
	Ast *op1 = expr->child;

	Value *rval = eval(op1);

	// NATIVE_NUMBER always results in "float". This doesn't.
	value_set(rval, - (rval->type == T_INT) ? rval->as_int : rval->as_float);

	return rval;
}

static
Value *
_not(const Ast *expr) {
	Ast *op1 = expr->child;

	Value *rval = eval(op1);

	value_set_bool(rval, ! rval->as_bool);

	return rval;
}

static
Value *
_eq(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	if(v1->type != v2->type) {
		value_set(rval, false);
	} else {
		switch(v1->type) {
		case T_BOOL:
			value_set_bool(rval, v1->as_bool == v2->as_bool);
			break;
		case T_INT:
			value_set_bool(rval, v1->as_int == v2->as_int);
			break;
		case T_FLOAT:
			value_set_bool(rval, abs((v1->as_float - v2->as_float)) < 0.05);
			break;
		case T_STRING:
			value_set_bool(rval, strcmp(v1->as_String, v2->as_String) == 0);
			break;
		}
	}
	value_free(v2);

	return rval;
}


static
Value *
_neq(const Ast *expr) {
	//Ast *op1 = expr->child;
	//Ast *op2 = expr->child->next;
	Value *rval = _eq(expr);

	value_set_bool(rval, !rval->as_bool);

	return rval;
}


static
Value *
_and(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);

	if(v1->as_bool == false) {
		return v1;
	}
	value_free(v1);

	return eval(op2);
}


static
Value *
_ior(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);

	if((v1->as_bool) == true) {
		return v1;
	}
	value_free(v1);

	return eval(op2);;
}


static
Value *
_lt(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	value_set_bool(rval, NATIVE_NUMBER(v1) < NATIVE_NUMBER(v2));
	value_free(v2);

	return rval;
}

static
Value *
_le(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	value_set_bool(rval, NATIVE_NUMBER(v1) <= NATIVE_NUMBER(v2));
	value_free(v2);

	return rval;
}

static
Value *
_ge(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	value_set_bool(rval, NATIVE_NUMBER(v1) >= NATIVE_NUMBER(v2));
	value_free(v2);

	return rval;
}

static
Value *
_gt(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	value_set_bool(rval, NATIVE_NUMBER(v1) > NATIVE_NUMBER(v2));
	value_free(v2);

	return rval;
}

static
Value *
_add(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, NATIVE_NUMBER(v1) + NATIVE_NUMBER(v2));
		break;
	case T_FLOAT:
		value_set_float(rval, NATIVE_NUMBER(v1) + NATIVE_NUMBER(v2));
		break;
	case T_STRING:
		// TODO String concat?
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_sub(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, NATIVE_NUMBER(v1) - NATIVE_NUMBER(v2));
		break;
	case T_FLOAT:
		value_set_float(rval, NATIVE_NUMBER(v1) - NATIVE_NUMBER(v2));
		break;
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_mul(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, NATIVE_NUMBER(v1) * NATIVE_NUMBER(v2));
		break;
	case T_FLOAT:
		value_set_float(rval, NATIVE_NUMBER(v1) * NATIVE_NUMBER(v2));
		break;
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_div(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, NATIVE_NUMBER(v1) / NATIVE_NUMBER(v2));
		break;
	case T_FLOAT:
		value_set_float(rval, NATIVE_NUMBER(v1) / NATIVE_NUMBER(v2));
		break;
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_pow(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, pow(NATIVE_NUMBER(v1),  NATIVE_NUMBER(v2)));
		break;
	case T_FLOAT:
		value_set_float(rval, powf(NATIVE_NUMBER(v1),  NATIVE_NUMBER(v2)));
		break;
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_mod(const Ast *expr) {
	Ast *op1 = expr->child;
	Ast *op2 = expr->child->next;

	Value *v1 = eval(op1);
	Value *v2 = eval(op2);
	Value *rval = v1;

	switch(expr->eval_type) {
	case T_INT:
		value_set_int(rval, v1->as_int % v2->as_int);
		break;
	default:
		pty(expr->eval_type);
		assert(false);
	}
	value_free(v2);

	return rval;
}

static
Value *
_bool(const Ast *expr) {
	Value *rval = value_new();
	char *s = expr->value;
	if(s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e' && s[4] == '\0') {
		// "true"
		value_set_bool(rval, true);
	} else if(s[0] == 'f' && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' && s[5] == '\0') {
		// "false"
		value_set_bool(rval, false);
	} else {
		assert(false);
	}

	return rval;
}

static
Value *
_int(const Ast *expr) {
	Value *rval = value_new();
	value_set_int(rval, atoi(expr->value));

	return rval;
}

static
Value *
_float(const Ast *expr) {
	Value *rval = value_new();
	value_set_float(rval, atof(expr->value));

	return rval;
}

static
Value *
_string(const Ast *expr) {
}

static
Value *
_set(const Ast *expr) {
}

static
Value *
_relation(const Ast *expr) {
}

