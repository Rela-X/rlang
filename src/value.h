#ifndef _VALUE_H
#define _VALUE_H

#include <stdbool.h>

#include <relafix.h>

#include "types.h"

typedef struct _value Value;
struct _value {
	Type type;
	union {
		bool            as_bool;
		int             as_int;
		double          as_float;
		char            *as_String;
		rf_Set          *as_Set;
		rf_Relation     *as_Relation;
	};
};

Value * value_new();

void value_copy(Value *dst, Value *src);

void value_set_void(Value *);
void value_set_bool(Value *, bool);
void value_set_int(Value *, int);
void value_set_float(Value *, double);
void value_set_string(Value *, char *);
void value_set_set(Value *, rf_Set *);
void value_set_relation(Value *, rf_Relation *);

void value_free(Value *);

#endif
