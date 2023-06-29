
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tables.h"
#include "types.h"

// Strings Table
// ----------------------------------------------------------------------------

#define STRING_MAX_SIZE 128
#define STRINGS_TABLE_MAX_SIZE 100
#define GLOBAL_SCOPE 0

struct str_table {
    char t[STRINGS_TABLE_MAX_SIZE][STRING_MAX_SIZE];
    int size;
};

StrTable* create_str_table() {
    StrTable *st = malloc(sizeof * st);
    st->size = 0;
    return st;
}

int add_string(StrTable* st, char* s) {
    for (int i = 0; i < st->size; i++) {
        if (strcmp(st->t[i], s) == 0) {
            return i;
        }
    }
    strcpy(st->t[st->size], s);
    int idx_added = st->size;
    st->size++;
    return idx_added;
}

char* get_string(StrTable* st, int i) {
    return st->t[i];
}

void print_str_table(StrTable* st) {
    printf("Strings table:\n");
    for (int i = 0; i < st->size; i++) {
        printf("Entry %d -- %s\n", i, get_string(st, i));
    }
}

void free_str_table(StrTable* st) {
    free(st);
}

// Variables Table
// ----------------------------------------------------------------------------

#define VARIABLE_MAX_SIZE 128
#define VARIABLES_TABLE_MAX_SIZE 100

typedef struct {
  char name[VARIABLE_MAX_SIZE];
  int scope;
  int line;
  Type type;
  Type array_type;
  int array_dimension;
} Entry;

struct var_table {
    Entry t[VARIABLES_TABLE_MAX_SIZE];
    int size;
};

VarTable* create_var_table() {
    VarTable *vt = malloc(sizeof * vt);
    vt->size = 0;
    return vt;
}

int lookup_for_create_var(VarTable* vt, char* s, int scope) {
    for (int i = 0; i < vt->size; i++) {
        if (strcmp(vt->t[i].name, s) == 0 && scope == get_scope(vt, i))/*variable in the same scope passed or global scope*/  {
            return i;
        }
    }
    return -1;
}

int lookup_var(VarTable* vt, char* s, int scope) {
    for (int i = 0; i < vt->size; i++) {
        if (strcmp(vt->t[i].name, s) == 0 &&
	    (scope == get_scope(vt, i) || get_scope(vt,i) == GLOBAL_SCOPE))/*variable in the same scope passed or global scope*/  {
            return i;
        }
    }
    return -1;
}

int add_to_var_table(VarTable* vt, char* s, int line, Type type, Type array_type, int scope, int dimension) {
    strcpy(vt->t[vt->size].name, s);
    vt->t[vt->size].line = line;
    vt->t[vt->size].type = type;
    vt->t[vt->size].array_type = array_type;
    vt->t[vt->size].scope = scope;
    vt->t[vt->size].array_dimension = dimension;
    int idx_added = vt->size;
    vt->size++;
    return idx_added;
}

int add_var(VarTable* vt, char* s, int line, Type type, int scope) {
	return add_to_var_table(vt, s, line, type, type, scope, 0);
}

int add_array(VarTable* vt, char* s, int line, Type type, int scope, int dimension) {
	return add_to_var_table(vt, s, line, ARRAY, type, scope, dimension);
}

char* get_name(VarTable* vt, int i) {
    return vt->t[i].name;
}

int get_line(VarTable* vt, int i) {
    return vt->t[i].line;
}

Type get_array_type(VarTable* vt, int i) {
    return vt->t[i].array_type;
}

Type get_type(VarTable* vt, int i) {
    return vt->t[i].type;
}

int get_scope(VarTable* vt, int i) {
    return vt->t[i].scope;
}

void print_var_table(char* name, VarTable* vt) {
    printf("%s table:\n", name);
    for (int i = 0; i < vt->size; i++) {
         printf("Entry %d -- name: %s, line: %d, type: %s, scope: %d\n", i,
                get_name(vt, i), get_line(vt, i), get_text(get_type(vt, i)), get_scope(vt, i));
    }
}

void free_var_table(VarTable* vt) {
    free(vt);
}

// Function Table
// ----------------------------------------------------------------------------

#define FUNCTION_MAX_SIZE 128
#define FUNCTION_TABLE_MAX_SIZE 100

typedef struct {
  char name[FUNCTION_MAX_SIZE];
  int scope;
  int line;
  Type type;
  Type *param_types;
  AST* ast_start;
  int num_param;
} FuncEntry;

struct func_table {
    FuncEntry t[FUNCTION_TABLE_MAX_SIZE];
    int size;
};

FuncTable* create_func_table() {
    FuncTable *vt = malloc(sizeof * vt);
    vt->size = 0;
    return vt;
}

int get_func_table_size(FuncTable* vt) {
	return vt->size;
}

int lookup_for_create_func(FuncTable* vt, char* s, int scope) {
    for (int i = 0; i < vt->size; i++) {
        if (strcmp(vt->t[i].name, s) == 0 && scope == get_func_scope(vt, i))/*variable in the same scope passed or global scope*/  {
            return i;
        }
    }
    return -1;
}

int lookup_func(FuncTable* vt, char* s, int scope) {
    for (int i = 0; i < vt->size; i++) {
        if (strcmp(vt->t[i].name, s) == 0 &&
	    (scope == get_func_scope(vt, i) || get_func_scope(vt,i) == GLOBAL_SCOPE))/*variable in the same scope passed or global scope*/  {
            return i;
        }
    }
    return -1;
}

int add_func(FuncTable* vt, char* s, int line, Type type, int scope) {
    strcpy(vt->t[vt->size].name, s);
    vt->t[vt->size].line = line;
    vt->t[vt->size].type = type;
    vt->t[vt->size].scope = scope;
    int idx_added = vt->size;
    vt->size++;
    return idx_added;
}

void add_func_params(FuncTable* vt, int i, Type *param_types, int num_param) {
    vt->t[i].num_param = num_param;
    vt->t[i].param_types = malloc(num_param * sizeof(Type));
}

char* get_func_name(FuncTable* vt, int i) {
    return vt->t[i].name;
}

int get_func_line(FuncTable* vt, int i) {
    return vt->t[i].line;
}

Type get_func_type(FuncTable* vt, int i) {
    return vt->t[i].type;
}

int get_func_scope(FuncTable* vt, int i) {
    return vt->t[i].scope;
}

int get_func_num_params(FuncTable* vt, int i) {
    return vt->t[i].num_param;
}

void set_func_ast_start(FuncTable* ft, int i, AST* ast) {
	ft->t[i].ast_start = ast;
}

AST* get_func_ast_start(FuncTable* ft, int i) {
	return ft->t[i].ast_start;
}

void print_func_table(char* name, FuncTable* vt) {
    printf("%s table:\n", name);
    for (int i = 0; i < vt->size; i++) {
		printf("Entry %d -- name: %s, line: %d, return type: %s, scope: %d, num params: %d\n",
				i, get_func_name(vt, i), get_func_line(vt, i), get_text(get_func_type(vt, i)),
				get_func_scope(vt, i), get_func_num_params(vt, i));
    }
}

void free_func_table(FuncTable* vt) {
	for(int i = 0; i < vt->size; i++) {
    	free(vt->t[i].param_types);
	}
    free(vt);
}
