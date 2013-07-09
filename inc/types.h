#ifndef TYPES_H_
#define TYPES_H_

/* not including T_NONE */
#define NTYPES 6

enum _rlang_type {
	T_NONE = -1,
	T_VOID, T_BOOL, T_INT, T_FLOAT, T_STRING, T_R
};
typedef enum _rlang_type Type;

void    print_type(Type type);

#endif
