#ifndef VARIABLE_H_
#define VARIABLE_H_

typedef struct _variable Variable;
struct _variable {
        char	*name;
        // TODO type
        union {
                bool bool;
                int int;
                float float;
                char *string;
        } value;

        Variable *next;
};

Variable *      variable_new(const char *name /*, type */);
Variable *      variable_new_bool(const char *name /*, type */, bool value);

void            variable_free(Variable *var);

#endif
