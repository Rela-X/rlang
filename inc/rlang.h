#ifndef RLANG_H_
#define RLANG_H_

#include <stdbool.h>

#include "rlang.tab.h"

bool reduce_not(bool a, YYLTYPE *loc);
bool reduce_eq(bool a, bool b, YYLTYPE *loc);
bool reduce_and(bool a, bool b, YYLTYPE *loc);
bool reduce_ior(bool a, bool b, YYLTYPE *loc);
bool reduce_xor(bool a, bool b, YYLTYPE *loc);

bool reduce_eq2(int a, int b, YYLTYPE *loc);
bool reduce_lt(int a, int b, YYLTYPE *loc);
bool reduce_le(int a, int b, YYLTYPE *loc);
bool reduce_ge(int a, int b, YYLTYPE *loc);
bool reduce_gt(int a, int b, YYLTYPE *loc);

int reduce_neg(int a, YYLTYPE *loc);
int reduce_add(int a, int b, YYLTYPE *loc);
int reduce_sub(int a, int b, YYLTYPE *loc);
int reduce_mul(int a, int b, YYLTYPE *loc);
int reduce_div(int a, int b, YYLTYPE *loc);
int reduce_pow(int a, int b, YYLTYPE *loc);
int reduce_mod(int a, int b, YYLTYPE *loc);

#endif /* RLANG_H_ */
