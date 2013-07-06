
#ifndef SYMBOL_H_
#define SYMBOL_H_

#include "types.h"

typedef struct _symbol Symbol;
struct _symbol {
        char	        *name;
        RLangType       eval_type;

        Symbol          *next;
};

Symbol *        symbol_new(const char *name);

void            symbol_free(Symbol *symbol);

#endif
