
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tables.h"
#include "types.h"

#define CHILDREN_LIMIT 20 // Don't try this at home, kids... :P

struct node {
    NodeKind kind;
    union {
        int   as_int;
        float as_float;
    } data;
    Type type;
    int count;
    AST* child[CHILDREN_LIMIT];
};

AST* new_node(NodeKind kind, int data, Type type) {
    AST* node = malloc(sizeof * node);
    node->kind = kind;
    node->data.as_int = data;
    node->type = type;
    node->count = 0;
    for (int i = 0; i < CHILDREN_LIMIT; i++) {
        node->child[i] = NULL;
    }
    return node;
}

void add_child(AST *parent, AST *child) {
    if (parent->count == CHILDREN_LIMIT) {
        fprintf(stderr, "Cannot add another child!\n");
        exit(1);
    }
    parent->child[parent->count] = child;
    parent->count++;
}

AST* get_child(AST *parent, int idx) {
    return parent->child[idx];
}

AST* new_subtree(NodeKind kind, Type type, int child_count, ...) {
    if (child_count > CHILDREN_LIMIT) {
        fprintf(stderr, "Too many children as arguments!\n");
        exit(1);
    }

    AST* node = new_node(kind, 0, type);
    va_list ap;
    va_start(ap, child_count);
    for (int i = 0; i < child_count; i++) {
        add_child(node, va_arg(ap, AST*));
    }
    va_end(ap);
    return node;
}

NodeKind get_kind(AST *node) {
    return node->kind;
}

int get_data(AST *node) {
    return node->data.as_int;
}

void set_float_data(AST *node, float data) {
    node->data.as_float = data;
}

float get_float_data(AST *node) {
    return node->data.as_float;
}

Type get_node_type(AST *node) {
    return node->type;
}

int get_child_count(AST *node) {
    return node->count;
}

void free_tree(AST *tree) {
    if (tree == NULL) return;
    for (int i = 0; i < tree->count; i++) {
        free_tree(tree->child[i]);
    }
    free(tree);
}

// Dot output.

int nr;

extern VarTable *var_table;
extern FuncTable *func_table;

char* kind2str(NodeKind kind) {
    switch(kind) {
        case ASSIGN_NODE:   	return ":=";
        case EQ_NODE:       	return "=";
        case BLOCK_NODE:    	return "block";
        case IF_NODE:       	return "if";
        case INT_VAL_NODE:  	return "";
        case LT_NODE:       	return "<";
        case GT_NODE:       	return ">";
        case NOT_NODE:      	return "!";
        case NEG_NODE:      	return "--";
        case AND_NODE:      	return "&&";
        case OR_NODE:      		return "||";
        case MINUS_NODE:    	return "-";
        case OVER_NODE:     	return "/";
        case PLUS_NODE:     	return "+";
        case PROGRAM_NODE:  	return "program";
        case FLOAT_VAL_NODE: 	return "";
        case WHILE_NODE:   		return "repeat";
        case STRING_VAL_NODE: 	return "";
        case TIMES_NODE:    	return "*";
        case FUNC_DECL_NODE: 	return "func_decl";
        case RETURN_NODE: 		return "return";
        case VAR_DECL_NODE: 	return "var_decl";
        case VAR_LIST_NODE: 	return "var_list";
        case VAR_USE_NODE:  	return "var_use";
        case I2R_NODE:      	return "I2R";
        case R2I_NODE:      	return "R2I";
        default:            	return "ERROR!!";
    }
}

int has_data(NodeKind kind) {
    switch(kind) {
		case VOID_VAL_NODE:
        case INT_VAL_NODE:
        case FLOAT_VAL_NODE:
        case STRING_VAL_NODE:
        case VAR_DECL_NODE:
        case VAR_USE_NODE:
        case FUNC_DECL_NODE:
        case FUNC_USE_NODE:
        case ARRAY_USE_NODE:
            return 1;
        default:
            return 0;
    }
}

int print_node_dot(AST *node) {
    int my_nr = nr++;

    fprintf(stderr, "node%d[label=\"", my_nr);
    if (node->type != VOID_TYPE) {
        fprintf(stderr, "(%s) ", get_text(node->type));
    }
    if (node->kind == VAR_DECL_NODE || node->kind == VAR_USE_NODE || node->kind == ARRAY_USE_NODE) {
        fprintf(stderr, "%s@", get_name(var_table, node->data.as_int));
	} else if(node->kind == FUNC_DECL_NODE || node->kind == FUNC_USE_NODE) {
        fprintf(stderr, "%s@", get_func_name(func_table, node->data.as_int));
	} else {
        fprintf(stderr, "%s", kind2str(node->kind));
    }
    if (has_data(node->kind)) {
        if (node->kind == FLOAT_VAL_NODE) {
            fprintf(stderr, "%.2f", node->data.as_float);
        } else if (node->kind == STRING_VAL_NODE) {
            fprintf(stderr, "@%d", node->data.as_int);
		} else if (node->kind == VOID_VAL_NODE) {
            fprintf(stderr, "@VOID");
		} else {
            fprintf(stderr, "%d", node->data.as_int);
        }
    }
    fprintf(stderr, "\"];\n");

    for (int i = 0; i < node->count; i++) {
        int child_nr = print_node_dot(node->child[i]);
        fprintf(stderr, "node%d -> node%d;\n", my_nr, child_nr);
    }
    return my_nr;
}

void print_dot(AST *tree) {
    nr = 0;
    fprintf(stderr, "digraph {\ngraph [ordering=\"out\"];\n");
    print_node_dot(tree);
    fprintf(stderr, "}\n");
}
