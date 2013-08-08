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

int
main() {
	yydebug = 0;

	Ast *root;
	int value;
	value = yyparse(&root);

	pt(root);

	ast_annotate_scopes(root);
root->scope = root->child->scope;
	ast_annotate_symbols(root);

	ast_annotate_types(root);
	ast_validate_types(root);

	ast_execute(root);

	return value;
}
