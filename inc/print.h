#ifndef _PRINT_H
#define _PRINT_H

#include <stdio.h>

#include "ast.h"

#ifndef foobarbaz
#define pn(n)  do { print_node(stdout, n); printf("\n"); } while(0);
#define pns(n) do { print_node_symbolinfo(stdout, n); printf("\n"); } while(0);
#define pnt(n) do { print_node_typeinfo(stdout, n); printf("\n"); } while(0);

#define pt(n)  do { print_tree(stdout, n); printf("\n"); } while(0);
#define pts(n) do { print_tree_symbolinfo(stdout, n); printf("\n"); } while(0);
#define ptt(n) do { print_tree_typeinfo(stdout, n); printf("\n"); } while(0);

#define pty(n) do { print_type(stdout, n); printf("\n"); } while(0);

#endif

void print_node(FILE *, const Ast *);
void print_node_symbol(FILE *, const Ast *);
void print_node_type(FILE *, const Ast *);

void print_tree(FILE *, const Ast *);
void print_tree_symbolinfo(FILE *, const Ast *);
void print_tree_typeinfo(FILE *, const Ast *);

void print_type(FILE *, const Type);

#endif
