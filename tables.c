
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

int get_str_table_size(StrTable* st) {
	return st->size;
}

StrTable* create_str_table() {
    StrTable *st = malloc(sizeof * st);
    st->size = 0;
	add_string(st, "%s\0");
	add_string(st, "%d\0");
	add_string(st, "%f\0");
    return st;
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
  int array_size;
  int array_dimension;
  int relative_pos;
} Entry;

struct var_table {
    Entry t[VARIABLES_TABLE_MAX_SIZE];
    int size;
};

int get_var_table_size(VarTable* vt) {
	return vt->size;
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

int add_to_var_table(VarTable* vt, char* s, int line, Type type, Type array_type, int scope, int dimension, int relative_pos) {
    strcpy(vt->t[vt->size].name, s);
    vt->t[vt->size].line = line;
    vt->t[vt->size].type = type;
    vt->t[vt->size].array_type = array_type;
    vt->t[vt->size].scope = scope;
    vt->t[vt->size].array_dimension = dimension;
    vt->t[vt->size].relative_pos = relative_pos;
    int idx_added = vt->size;
    vt->size++;
    return idx_added;
}

int add_var(VarTable* vt, char* s, int line, Type type, int scope, int relative_pos) {
	return add_to_var_table(vt, s, line, type, type, scope, 0, relative_pos);
}

int add_array(VarTable* vt, char* s, int line, Type type, int scope, int dimension, int relative_pos, int size) {
	int pos = add_to_var_table(vt, s, line, ARRAY, type, scope, dimension, relative_pos);
	set_array_size(vt, pos, size);
	return pos;
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

int get_var_offset(VarTable* vt, int i) {
	return vt->t[i].relative_pos;
}

void set_array_size(VarTable* vt, int i, int size) {
	vt->t[i].array_size = size;
}

int get_array_size(VarTable* vt, int i) {
	return vt->t[i].array_size;
}

VarTable* create_var_table() {
    VarTable *vt = malloc(sizeof * vt);
    vt->size = 0;
	add_var(vt, "elem", 0, STR_TYPE, 1, 0);
    return vt;
}

void print_var_table(char* name, VarTable* vt) {
    printf("%s table:\n", name);
    for (int i = 0; i < vt->size; i++) {
         printf("Entry %d -- name: %s, line: %d, type: %s, scope: %d, relative: %d\n", i,
                get_name(vt, i), get_line(vt, i), get_text(get_type(vt, i)), get_scope(vt, i), get_var_offset(vt, i));
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
  int builtin;
  Type type;
  Type *param_types;
  AST* ast_start;
  int num_param;
} FuncEntry;

struct func_table {
    FuncEntry t[FUNCTION_TABLE_MAX_SIZE];
	int num_vars[FUNCTION_TABLE_MAX_SIZE + 1];
    int size;
};

int get_func_table_size(FuncTable* ft) {
	return ft->size;
}

int get_func_num_vars(FuncTable* ft, int i) {
	return ft->num_vars[i];
}

void add_var_to_func(FuncTable* ft, int i) {
	ft->num_vars[i]++;
}

void add_array_to_func(FuncTable* ft, int i, int array_size) {
	ft->num_vars[i] += array_size;
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

int add_func_builtin(FuncTable* vt, char* s, int line, Type type, int scope) {
	int pos = add_func(vt, s, line, type, scope);
	vt->t[pos].builtin = 1;
	return pos;
}

int add_func(FuncTable* vt, char* s, int line, Type type, int scope) {
    strcpy(vt->t[vt->size].name, s);
    vt->t[vt->size].line = line;
    vt->t[vt->size].type = type;
    vt->t[vt->size].scope = scope;
    int idx_added = vt->size;
	vt->t[vt->size].builtin = 0;
    vt->size++;
    return idx_added;
}

void add_func_params(FuncTable* vt, int i, Type *param_types, int num_param) {
    vt->t[i].num_param = num_param;
    vt->t[i].param_types = malloc(num_param * sizeof(Type));
	for(int i = 0; i < num_param; i++) {
		vt->t[i].param_types[i] = param_types[i];
	}
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

int get_func_is_builtin(FuncTable* ft, int i) {
	return ft->t[i].builtin;
}

void add_builtin_functions(FuncTable* ft) {
	int pos = add_func_builtin(ft, "printf", 0, VOID_TYPE, 0);
	Type args[] = {STR_TYPE};
	add_func_params(ft, pos, args, 1);

	pos = add_func_builtin(ft, "scanf", 0, VOID_TYPE, 0);
	args[0] = INT_TYPE;
	add_func_params(ft, pos, args, 1);
}

FuncTable* create_func_table() {
    FuncTable *ft = malloc(sizeof * ft);
    ft->size = 0;
	add_builtin_functions(ft);
    return ft;
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
