#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <stdbool.h>

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
        bool            assigned;
        bool            read;

        Type            eval_type;

        Symbol          *next;
};

Symbol *        symbol_new(SymbolClass class, const char *name);

void            symbol_free(Symbol *symbol);

#endif

