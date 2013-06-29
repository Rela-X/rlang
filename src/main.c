#include "ast.h"
#include "scope.h"

extern int yyparse(Ast *ast);
extern int yydebug;

int
main() {
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	ast_print_tree(root);

	Scope *global_scope = scope_new(NULL);
	check_symbols(root, global_scope);

	return value;
}
