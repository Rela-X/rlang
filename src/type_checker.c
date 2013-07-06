#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "types.h"

static RLangType get_type(Ast *);
static RLangType get_type_identifier(Ast *);
static RLangType get_type_declaration(Ast *);

static RLangType *arithmetic_result_type_table[] = {
	                  /* void */ /* boolean */ /* integer */ /* float */ /* String */ /* R */
	/* void */      { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
	/* boolean */   { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
	/* integer */   { rl_VOID,   rl_VOID,      rl_INT,       rl_FLOAT,   rl_VOID,     rl_VOID },
	/* float */     { rl_VOID,   rl_VOID,      rl_FLOAT,     rl_FLOAT,   rl_VOID,     rl_VOID },
	/* String */    { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
	/* R */         { rl_VOID,   rl_VOID,      rl_VOID,      rl_VOID,    rl_VOID,     rl_VOID },
};

void
check_types(Ast *ast) {
	ast->eval_type = get_type(ast);

ast_print_node(ast); printf(" evaluates to "); print_type(ast->eval_type); printf("\n");

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
	case NT_ADD: case NT_SUB: case NT_MUL: case NT_DIV: case NT_POW: case NT_MOD:
		return get_type(ast->child); // TODO
	default:
		return rl_VOID;
	}
}

static
RLangType
get_type_identifier(Ast *id) {
	Symbol *sy = scope_resolve(id->scope, id->value);

	return sy->eval_type;
}

static
RLangType
get_type_declaration(Ast *declaration) {
	Ast *type = declaration->child;
	Ast *id = declaration->child->next;

	/* set the type for the SYMBOL, not just for the id-node */
	id->symbol->eval_type = get_type_identifier(type);

	return rl_VOID;
}

RLangType
resultType(Ast *a, NodeType operator, Ast *b) {
}

RLangType
promoteFromTo(RLangType from, NodeType operator, RLangType to) {
}

