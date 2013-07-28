#include <stdlib.h>

#include "ast.h"
#include "scope.h"

extern int yyparse(Ast **ast);
extern int yydebug;

static
void
init_global_scope(Scope *global_scope) {
	Symbol *sy;

	sy = symbol_new(S_TYPE, "bool");
	sy->eval_type = T_BOOL;
	scope_define(global_scope, sy);

	sy = symbol_new(S_TYPE, "int");
	sy->eval_type = T_INT;
	scope_define(global_scope, sy);

	sy = symbol_new(S_TYPE, "float");
	sy->eval_type = T_FLOAT;
	scope_define(global_scope, sy);

	sy = symbol_new(S_TYPE, "String");
	sy->eval_type = T_STRING;
	scope_define(global_scope, sy);

	sy = symbol_new(S_TYPE, "Set");
	sy->eval_type = T_SET;
	scope_define(global_scope, sy);

	sy = symbol_new(S_TYPE, "R");
	sy->eval_type = T_R;
	scope_define(global_scope, sy);
}

int
main() {
float a = 1.23;
float b = 3.45;
void *x = &a;
void *y = &b;
printf("%f\n", *(float *)x + *(float *)y);
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	ast_print_tree(root);
	printf("\n");

	Scope *global_scope = scope_new(NULL);
	init_global_scope(global_scope);

	root->scope = global_scope;
	ast_annotate_scopes(root);
	ast_annotate_symbols(root);

	ast_annotate_types(root);
	ast_validate_types(root);

	ast_execute(root);

	return value;
}
