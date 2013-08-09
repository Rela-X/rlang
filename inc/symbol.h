#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <stdbool.h>

typedef struct _ast Ast;
typedef struct _scope Scope;
#include "types.h"

enum _symbol_class {
	S_NONE = -1,
	S_TYPE, S_VARIABLE, S_FUNCTION
};
typedef enum _symbol_class SymbolClass;

typedef struct _symbol Symbol;
struct _symbol {
        SymbolClass     class;
        char            *name;

        Type            eval_type;

        // S_VARIABLE
        bool            assigned;
        bool            read;

        // S_FUNCTION
        Scope           *args;
        Ast             *code;

        Symbol          *next;
};

Symbol *        symbol_new(SymbolClass class, const char *name);

void            symbol_free(Symbol *symbol);

#endif

