#ifndef _VALUE_H
#define _VALUE_H

#include <stdbool.h>

#include "types.h"

typedef struct _value Value;
struct _value {
	Type type;
	union {
		bool    as_bool;
		int     as_int;
		double  as_float;
		char    *as_String;
	};
};

Value * value_new();

void value_copy(Value *dst, Value *src);

#define value_set(v, nv) _Generic((nv), \
	int : value_set_int, \
	float : value_set_float, \
	double : value_set_float, \
	char * : value_set_string \
	) (v, nv)

void value_set_bool(Value *, bool);
void value_set_int(Value *, int);
void value_set_float(Value *, double);
void value_set_string(Value *, char *);
/*
void value_set_set(Value *, rf_Set *);
void value_set_r(Value *, rf_R *);
*/

void value_free(Value *);

#endif
