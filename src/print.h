#ifndef _PRINT_H
#define _PRINT_H

#include "rlang.tab.h"

#include <stdio.h>
#include <stdarg.h>

#include "ast.h"
#include "value.h"

#ifndef foobarbaz

#define pn(n)  do { dprint_node(2, n); dprintf(2, "\n"); } while(0)
#define pns(n) do { dprint_node_symbolinfo(2, n); dprintf(2, "\n"); } while(0)
#define pnt(n) do { dprint_node_typeinfo(2, n); dprintf(2, "\n"); } while(0)

#define pt(n)  do { dprint_tree(2, n); dprintf(2, "\n"); } while(0)
#define pts(n) do { dprint_tree_symbolinfo(2, n); dprintf(2, "\n"); } while(0)
#define ptt(n) do { dprint_tree_typeinfo(2, n); dprintf(2, "\n"); } while(0)

#define pty(ty) do { dprint_type(2, ty); dprintf(2, "\n"); } while(0)

#define pv(v) do { dprint_value(2, v); dprintf(2, "\n"); } while(0)

#endif

#define log(...)	dprintf(2, __VA_ARGS__)
void err(const char *format, ...);
void err_at(const YYLTYPE *yyloc, const char *format, ...);

#define print_node(AST)         dprint_node(2, AST)
#define print_node_symbol(AST)  dprint_node_symbol(2, AST)
#define print_node_type(AST)    dprint_node_type(2, AST)

#define print_tree(AST)                 dprint_tree(2, AST)
#define print_tree_symbolinfo(AST)      dprint_tree_symbolinfo(2, AST)
#define print_tree_typeinfo(AST)        dprint_tree_typeinfo(2, AST)

#define print_type(TYPE)        dprint_type(2, TYPE)

#define print_value(VALUE)      dprint_value(2, VALUE)

void dprint_node(int fd, const Ast *);
void dprint_node_symbol(int fd, const Ast *);
void dprint_node_type(int fd, const Ast *);

void dprint_tree(int fd, const Ast *);
void dprint_tree_symbolinfo(int fd, const Ast *);
void dprint_tree_typeinfo(int fd, const Ast *);

void dprint_type(int fd, const Type);

void dprint_value(int fd, const Value *);

#endif
