#ifndef SCOPE_H_
#define SCOPE_H_

#include "variable.h"

typedef Scope struct _scope;
struct _scope {
        /** Parent of this Scope. May be null */
        Scope *parent;
        /** Variables as a linked-list */
        Variable *variables;
}

Scope *         scope_new(const Scope *parent);
Variable *      scope_resolve(const Scope *scope, const char *varname);
void            scope_push_var(const Scope *scope, const Variable *variable);
void            scope_free(Scope *scope);

#endif
