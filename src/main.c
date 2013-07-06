#include <stdlib.h>

#include "ast.h"
#include "scope.h"

extern int yyparse(Ast **ast);
extern int yydebug;

int
main() {
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	ast_print_tree(root);
	printf("\n");

	Scope *global_scope = scope_new(NULL);
	Symbol *sy;
	scope_define(global_scope, sy = symbol_new("bool"));
	sy->eval_type = rl_BOOL;
	scope_define(global_scope, sy = symbol_new("int"));
	sy->eval_type = rl_INT;
	scope_define(global_scope, sy = symbol_new("float"));
	sy->eval_type = rl_FLOAT;
	scope_define(global_scope, sy = symbol_new("String"));
	sy->eval_type = rl_STRING;
	scope_define(global_scope, sy = symbol_new("R"));
	sy->eval_type = rl_R;

	root->scope = global_scope;
	check_symbols(root);

	check_types(root);

	return value;
}
