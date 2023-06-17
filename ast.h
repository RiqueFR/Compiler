#ifndef AST_H
#define AST_H

#include "types.h"

/*
 * INT_VAL_NODE
 * FLOAT_VAL_NODE
 * STRING_VAL_NODE
 * VOID_VAL_NODE
 * BLOCK_NODE
 * PROGRAM_NODE block_node;
 * FUNC_DECL_NODE -> type = return_type; data = 0; list of var_use_node;
 * block_node; VAR_DECL_NODE -> type = decl_type; data = idx; node_exp;
 * WHILE_NODE -> type = VOID; data = 0; exp_node; block_node;
 * IF_NODE -> type = VOID; data = 0; exp_node; block_node; block_node;
 * ASSIGN_NODE -> type = VOID; data = 0; var_use/array_var_use; exp_node;
 * FUNC_USE_NODE -> type = return_type; data = 0; var_list_node;
 * RETURN_NODE -> type = exp_type; data = 0; exp_node;
 * NOT_NODE -> type = exp_type; data = 0; exp_node;
 * NEG_NODE -> type = exp_type; data = 0; exp_node;
 * AND_NODE -> type = INT; data = 0; exp_node; exp_node;
 * OR_NODE -> type = INT; data = 0; exp_node; exp_node;
 * LT_NODE -> type = INT; data = 0; exp_node; exp_node;
 * GT_NODE -> type = INT; data = 0; exp_node; exp_node;
 * EQ_NODE -> type = INT; data = 0; exp_node; exp_node;
 * TIMES_NODE -> type = exp_type; data = 0; exp_node; exp_node;
 * OVER_NODE -> type = exp_type; data = 0; exp_node; exp_node;
 * PLUS_NODE -> type = exp_type; data = 0; exp_node; exp_node;
 * MINUS_NODE -> type = exp_type; data = 0; exp_node; exp_node;
 */

typedef enum {
  PROGRAM_NODE,
  BLOCK_NODE,
  WHILE_NODE,
  IF_NODE,
  ASSIGN_NODE,
  NOT_NODE,
  EQ_NODE,
  LT_NODE,
  GT_NODE,
  AND_NODE,
  OR_NODE,
  NEG_NODE,
  PLUS_NODE,
  MINUS_NODE,
  TIMES_NODE,
  OVER_NODE,
  FUNC_DECL_NODE,
  FUNC_USE_NODE,
  RETURN_NODE,
  VAR_DECL_NODE,
  VAR_LIST_NODE,
  VAR_USE_NODE,
  ARRAY_USE_NODE,
  INT_VAL_NODE,
  FLOAT_VAL_NODE,
  STRING_VAL_NODE,
  VOID_VAL_NODE,

  I2R_NODE,
  R2I_NODE
} NodeKind;

struct node; // Opaque structure to ensure encapsulation.

typedef struct node AST;

AST *new_node(NodeKind kind, int data, Type type);

void add_child(AST *parent, AST *child);
AST *get_child(AST *parent, int idx);

AST *new_subtree(NodeKind kind, Type type, int child_count, ...);

NodeKind get_kind(AST *node);
char *kind2str(NodeKind kind);

int get_data(AST *node);
void set_float_data(AST *node, float data);
float get_float_data(AST *node);

Type get_node_type(AST *node);
int get_child_count(AST *node);

void print_tree(AST *ast);
void print_dot(AST *ast);

void free_tree(AST *ast);

#endif
