#include <stdlib.h>

#include "ast.h"
#include "builtins.h"
#include "print.h"
#include "scope.h"
#include "symbol.h"

extern int yyparse(Ast **ast);
extern int yydebug;

extern void ast_annotate_scopes(Ast *, Scope *);
extern int ast_annotate_symbols(Ast *);
extern void ast_annotate_types(Ast *);
extern int ast_validate_types(Ast *);
extern void ast_execute(Ast *);
extern void ast_cleanup(Ast *);

static
void
init_builtin_types(Scope *builtin_scope) {
#define def_type(name, type) do {               \
	Symbol *sy = symbol_new(S_TYPE, name);  \
	sy->eval_type = type;                   \
	scope_define(builtin_scope, sy);        \
	} while(0)

	def_type("void", T_VOID);
	def_type("bool", T_BOOL);
	def_type("int", T_INT);
	def_type("float", T_FLOAT);
	def_type("String", T_STRING);
	def_type("Set", T_SET);
	def_type("R", T_R);
#undef def_type
}

int
main() {
	yydebug = 0;

	Ast *root = NULL;
	int err = 0;
	if((err = yyparse(&root))) {
		assert(root == NULL);
		goto exit;
	}

	dprintf(2, "### AST:\n");
	pt(root);
	dprintf(2, "\n");

	Scope *builtin_scope = scope_new(NULL);
	init_builtin_types(builtin_scope);
	init_builtin_functions(builtin_scope);

	ast_annotate_scopes(root, builtin_scope);

	if((err = ast_annotate_symbols(root))) goto cleanup_ast_meta;

	ast_annotate_types(root);
	if((err = ast_validate_types(root))) goto cleanup_ast_meta;

	ast_execute(root);

cleanup_ast_meta:
	ast_cleanup(root);
	scope_free(builtin_scope);
cleanup_ast:
	ast_free(root);
exit:
	return err;
}
