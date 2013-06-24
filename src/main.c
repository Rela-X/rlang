extern int yyparse();
extern int yydebug;

int
main() {
        yydebug = 0;
        yyparse();
        return 0;
}
