/*
INT_VAL_NODE
FLOAT_VAL_NODE
STRING_VAL_NODE
VOID_VAL_NODE
BLOCK_NODE
PROGRAM_NODE block_node;
FUNC_DECL_NODE -> type = return_type; data = 0; list of var_use_node; block_node;
VAR_DECL_NODE -> type = decl_type; data = idx; node_exp;
WHILE_NODE -> type = VOID; data = 0; exp_node; block_node;
IF_NODE -> type = VOID; data = 0; exp_node; block_node; block_node;
ASSIGN_NODE -> type = VOID; data = 0; var_use/array_var_use; exp_node;
FUNC_USE_NODE -> type = return_type; data = 0; list of exp_node;
RETURN_NODE -> type = exp_type; data = 0; exp_node;
NOT_NODE -> type = exp_type; data = 0; exp_node;
NEG_NODE -> type = exp_type; data = 0; exp_node;
AND_NODE -> type = INT; data = 0; exp_node; exp_node;
OR_NODE -> type = INT; data = 0; exp_node; exp_node;
LT_NODE -> type = INT; data = 0; exp_node; exp_node;
GT_NODE -> type = INT; data = 0; exp_node; exp_node;
EQ_NODE -> type = INT; data = 0; exp_node; exp_node;
TIMES_NODE -> type = exp_type; data = 0; exp_node; exp_node;
OVER_NODE -> type = exp_type; data = 0; exp_node; exp_node;
PLUS_NODE -> type = exp_type; data = 0; exp_node; exp_node;
MINUS_NODE -> type = exp_type; data = 0; exp_node; exp_node;
*/
