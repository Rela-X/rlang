#ifndef TYPES_H_
#define TYPES_H_

/* not including rl_NONE */
#define NTYPES 6

enum _rlang_type {
	rl_NONE = -1,
	rl_VOID, rl_BOOL, rl_INT, rl_FLOAT, rl_STRING, rl_R
};
typedef enum _rlang_type RLangType;

void    print_type(RLangType type);

#endif
