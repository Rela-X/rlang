#ifndef _AST_H
#define _AST_H

#include "rlang.tab.h"

#include "scope.h"
#include "symbol.h"
#include "types.h"

enum _node_class {
	N_NONE = -1,
	N_BLOCK, N_IF, N_WHILE, N_DECLARATION, N_FUNCTION, N_FUNCTIONARGS,
	N_RETURN, N_CALL, N_CALLARGS, N_ASSIGNMENT, N_IDENTIFIER,
	N_NEG, N_NOT,
	N_EQ, N_NEQ,
	N_AND, N_IOR, N_XOR,
	N_LT, N_LE, N_GE, N_GT,
	N_ADD, N_SUB, N_MUL, N_DIV, N_POW, N_MOD,
	N_BOOLEAN, N_INTEGER, N_FLOAT, N_STRING, N_SET, N_R,
	N_RTABLE, N_RTABLEROW,
};
typedef enum _node_class NodeClass;

typedef struct _ast Ast;
struct _ast {
	NodeClass       class;
	/** node value. May be NULL */
	char            *value;
	/** linked list of child nodes */
	Ast             *child;
	/** next sibling in linked list */
	Ast             *next;

	/** source-code location, initialized { -1,-1,-1,-1 } */
	YYLTYPE		location;

	/** which scope does the node belong to */
	Scope           *scope;
	/** the symbol associated with this node. May be null */
	Symbol          *symbol;
	/** which type does the node evaluate to. Initialized as -1 */
	Type            eval_type;
	Type            promoted_type;
};

Ast *  ast_new(NodeClass type);
Ast *  ast_clone(const Ast *ast);

void   ast_append_child(Ast *ast, Ast *child);
#define ast_append_child_all(ast, ...) _ast_append_child_all(ast, __VA_ARGS__, NULL);
void   _ast_append_child_all(Ast *ast, ...);

void   ast_free(Ast *ast);

#endif
