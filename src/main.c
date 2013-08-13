#include <stdlib.h>

#include "ast.h"
#include "print.h"
#include "scope.h"
#include "symbol.h"

extern int yyparse(Ast **ast);
extern int yydebug;

extern void ast_annotate_scopes(Ast *);
extern void ast_annotate_symbols(Ast *);
extern void ast_annotate_types(Ast *);
extern void ast_validate_types(Ast *);
extern void ast_execute(Ast *);

static
void
init_builtin_scope(Scope *builtin_scope) {
#define def_type(name, type) do {               \
	Symbol *sy = symbol_new(S_TYPE, name);  \
	sy->eval_type = type;                   \
	scope_define(builtin_scope, sy);         \
	} while(0)

	def_type("void", T_VOID);
	def_type("bool", T_BOOL);
	def_type("int", T_INT);
	def_type("float", T_FLOAT);
	def_type("String", T_STRING);
	def_type("Set", T_SET);
	def_type("R", T_R);
}

int
main() {
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	pt(root);

	root->scope = scope_new(NULL);
	init_builtin_scope(root->scope);

	ast_annotate_scopes(root);

	ast_annotate_symbols(root);

	ast_annotate_types(root);
	ast_validate_types(root);

	ast_execute(root);

	Scope *scope, *free_ref;
	for(scope = root->scope; scope != NULL; scope = free_ref) {
		free_ref = scope->free_ref;
		scope_free(scope);
	}

	ast_free(root);

	return value;
}
