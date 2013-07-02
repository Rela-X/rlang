#include "ast.h"
#include "scope.h"

extern int yyparse(Ast **ast);
extern int yydebug;

#define NULL ((*void)0)

int
main() {
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	ast_print_tree(root);
	printf("\n");

	Scope *global_scope = scope_new(0);
	scope_define(global_scope, symbol_new("bool"));
	scope_define(global_scope, symbol_new("int"));
	scope_define(global_scope, symbol_new("float"));
	scope_define(global_scope, symbol_new("String"));
	scope_define(global_scope, symbol_new("R"));
	check_symbols(root, global_scope);

	return value;
}
