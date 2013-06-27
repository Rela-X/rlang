#include "ast.h"

extern int yyparse(Ast *ast);
extern int yydebug;

int
main() {
	Ast *root;
	yydebug = 0;
	int value;
	value = yyparse(&root);

	ast_print_tree(root);

	return value;
}
