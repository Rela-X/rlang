#ifndef SCOPE_H_
#define SCOPE_H_

#include "symbol.h"

typedef Scope struct _scope;
struct _scope {
        /** Parent of this Scope. May be null */
        Scope *parent;
        /** Variables as a linked-list */
        Variable *variables;
}

Scope *         scope_new(const Scope *parent);
void            scope_define(const Scope *scope, const Symbol *symbol);
Symbol *        scope_resolve(const Scope *scope, const char *name);
void            scope_free(Scope *scope);

#endif
