#ifndef AST_H_
#define AST_H_

enum _node_type {
	NT_BLOCK, NT_DECLARESTMT, NT_IFSTMT, NT_WHILESTMT,
	NT_ASSIGNMENT, 
	NT_NEG, NT_NOT,
	NT_EQ, NT_AND, NT_IOR, NT_XOR,
	NT_LT, NT_LE, NT_GE, NT_GT,
	NT_ADD, NT_SUB, NT_MUL, NT_DIV, NT_POW, NT_MOD,
	NT_IDENTIFIER, NT_BOOLEAN, NT_INTEGER, NT_FLOAT, NT_STRING, NT_R
};
typedef enum _node_type NodeType;

typedef struct _ast Ast;
struct _ast {
        NodeType        type;
	/** node value. May be NULL */
	char    *value;
	/** linked list of child nodes */
	Ast     *child;
        /** next sibling in linked list */
        Ast     *next;
};

Ast *  ast_new(NodeType type);
Ast *  ast_copy(Ast *ast);

void   ast_append_child(Ast *ast, Ast *child);
#define ast_append_child_all(ast, ...) _ast_append_child_all(ast, __VA_ARGS__, NULL);
void   _ast_append_child_all(Ast *ast, ...);

void   ast_print_node(Ast *ast);
void   ast_print_tree(Ast *ast);

void   ast_free(Ast *ast);

#endif
