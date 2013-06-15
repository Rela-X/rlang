#include "rlang.h"

#define debug

#ifdef debug
#define b2s(b) ((b) ? "true" : "false")
#endif

bool
reduce_not(bool a, YYLTYPE *loc) {
	bool r = !a;
#ifdef debug
	printf("eval not(%s) => %s\n", b2s(a), b2s(r));
#endif
	return r;
}

bool
reduce_eq(bool a, bool b, YYLTYPE *loc) {
	bool r = a == b;
#ifdef debug
	printf("eval eq(%s,%s) => %s\n", b2s(a), b2s(b), b2s(r));
#endif
	return r;
}

bool
reduce_and(bool a, bool b, YYLTYPE *loc) {
	bool r = a && b;
#ifdef debug
	printf("eval and(%s,%s) => %s\n", b2s(a), b2s(b), b2s(r));
#endif
	return r;
}

bool
reduce_ior(bool a, bool b, YYLTYPE *loc) {
	bool r = a || b;
#ifdef debug
	printf("eval ior(%s,%s) => %s\n", b2s(a), b2s(b), b2s(r));
#endif
	return r;
}

bool
reduce_xor(bool a, bool b, YYLTYPE *loc) {
	bool r = a != b;
#ifdef debug
	printf("eval xor(%s,%s) => %s\n", b2s(a), b2s(b), b2s(r));
#endif
	return r;
}

bool
reduce_eq2(int a, int b, YYLTYPE *loc) {
#ifdef debug
	printf("eq(%d,%d)\n", a, b);
#endif
	return false; // TODO
}

bool
reduce_lt(int a, int b, YYLTYPE *loc) {
	bool r = a < b;
#ifdef debug
	printf("eval lt(%d,%d) => %s\n", a, b, b2s(r));
#endif
	return r;
}

bool
reduce_le(int a, int b, YYLTYPE *loc) {
	bool r = a <= b;
#ifdef debug
	printf("eval le(%d,%d) => %s\n", a, b, b2s(r));
#endif
	return r;
}

bool
reduce_ge(int a, int b, YYLTYPE *loc) {
	bool r = a >= b;
#ifdef debug
	printf("eval ge(%d,%d) => %s\n", a, b, b2s(r));
#endif
	return r;
}

bool
reduce_gt(int a, int b, YYLTYPE *loc) {
	bool r = a > b;
#ifdef debug
	printf("eval gt(%d,%d) => %s\n", a, b, b2s(r));
#endif
	return r;
}


int
reduce_add(int a, int b, YYLTYPE *loc) {
	int r = a+b;
#ifdef debug
	printf("eval add(%d,%d) => %d\n", a, b, r);
#endif
	return r;
}

int
reduce_sub(int a, int b, YYLTYPE *loc) {
	int r = a-b;
#ifdef debug
	printf("eval sub(%d,%d) => %d\n", a, b, r);
#endif
	return r;
}

int
reduce_mul(int a, int b, YYLTYPE *loc) {
	int r = a*b;
#ifdef debug
	printf("eval mul(%d,%d) => %d\n", a, b, r);
#endif
	return r;
}

int
reduce_div(int a, int b, YYLTYPE *loc) {
	int r = a/b;
#ifdef debug
	printf("eval div(%d,%d) => %d\n", a, b, r);
#endif
	return r;
}

int
reduce_pow(int a, int b, YYLTYPE *loc) {
	int r = a*b; // TODO
#ifdef debug
	printf("eval pow(%d,%d) => %d //TODO\n", a, b, r);
#endif
	return r;
}

int
reduce_mod(int a, int b, YYLTYPE *loc) {
	int r = a%b;
#ifdef debug
	printf("eval mod(%d,%d) => %d\n", a, b, r);
#endif
	return r;
}
