#ifndef _TYPES_H
#define _TYPES_H

enum _type {
	T_NONE = -1,
	T_VOID, T_BOOL, T_INT, T_FLOAT, T_STRING, T_SET, T_R,
	NTYPES /* not including T_NONE */
};
typedef enum _type Type;

#endif

