#ifndef AST_H_
#define AST_H_

typedef struct _token Token;
struct _token {
	// enum yytokentype type;
	int type;
	char *value;
};

typedef struct _ast Ast;
struct _ast {
	Token   *token;
	/** linked list of child nodes */
	Ast     *child;
	/** next sibling in linked list */
	Ast     *next;
};

Token * token_new(int type, char *value);
void    token_free(Token *token);

Ast *  ast_new(Token *token);

void   ast_append_child(Ast *ast, Ast *child);
#define ast_append_child_all(ast, ...) _ast_append_child_all(ast, __VA_ARGS__, NULL);
void   _ast_append_child_all(Ast *ast, ...);

void   ast_print_node(Ast *ast);
void   ast_print_tree(Ast *ast);

void   ast_free(Ast *ast);

#endif
