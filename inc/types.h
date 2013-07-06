#ifndef TYPES_H_
#define TYPES_H_

enum _rlang_type {
	rl_VOID, rl_BOOL, rl_INT, rl_FLOAT, rl_STRING, rl_R
};
typedef enum _rlang_type RLangType;

void    print_type(RLangType type);

#endif
