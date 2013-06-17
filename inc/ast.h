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
	Token *token;
	/** null-terminated array of child nodes */
	Ast**children;
};

Token * token_new(int type, char *value);
void    token_free(Token *token);

Ast *  ast_new(Token *token);

void   ast_print_node(Ast *ast);
void   ast_print_tree(Ast *ast);

void   ast_free(Ast *ast);

#endif
