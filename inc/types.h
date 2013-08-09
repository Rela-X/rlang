#ifndef TYPES_H_
#define TYPES_H_

/* not including T_NONE */
#define NTYPES 7

enum _type {
	T_NONE = -1,
	T_VOID, T_BOOL, T_INT, T_FLOAT, T_STRING, T_SET, T_R
};
typedef enum _type Type;

#endif

