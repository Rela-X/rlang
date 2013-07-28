#ifndef AST_H_
#define AST_H_

#include "scope.h"
#include "types.h"

enum _node_class {
	N_NONE = -1,
	N_BLOCK, N_IF, N_WHILE, N_DECLARATION, N_FNDEF, N_FNARGS,
	N_ASSIGNMENT,
	N_NEG, N_NOT,
	N_EQ, N_NEQ,
	N_AND, N_IOR, N_XOR,
	N_LT, N_LE, N_GE, N_GT,
	N_ADD, N_SUB, N_MUL, N_DIV, N_POW, N_MOD,
	N_IDENTIFIER, N_BOOLEAN, N_INTEGER, N_FLOAT, N_STRING, N_SET, N_R
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

        /** which scope does the node belong to */
        Scope           *scope;
        /** the symbol associated with this node. May be null */
        Symbol          *symbol;
        /** which type does the node evaluate to. Initialized as -1 */
        Type            eval_type;
        Type            promoted_type;
};

Ast *  ast_new(NodeClass type);
Ast *  ast_copy(const Ast *ast);

void   ast_append_child(Ast *ast, Ast *child);
#define ast_append_child_all(ast, ...) _ast_append_child_all(ast, __VA_ARGS__, NULL);
void   _ast_append_child_all(Ast *ast, ...);

void   ast_print_node(const Ast *ast);
void   ast_print_tree(const Ast *ast);

void   ast_free(Ast *ast);

#endif
