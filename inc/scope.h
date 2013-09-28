#ifndef SCOPE_H_
#define SCOPE_H_

#include "symbol.h"

typedef struct _scope Scope;
struct _scope {
        /** Parent of this Scope. May be null */
        const Scope *parent;
        /** Symbol-table as a linked-list */
        Symbol *symbols;
};

Scope *         scope_new(Scope *parent);
void            scope_define(Scope *scope, Symbol *symbol);
Symbol *        scope_resolve(const Scope *scope, const char *name);
Symbol *        scope_resolve_recursive(const Scope *scope, const char *name);
void            scope_free(Scope *scope);

#endif

