
#ifndef TABLES_H
#define TABLES_H

#include "ast.h"
#include "types.h"

// Strings Table
// ----------------------------------------------------------------------------

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
struct str_table;
typedef struct str_table StrTable;

// Creates an empty strings table.
StrTable *create_str_table();

// Adds the given string to the table without repetitions.
// String 's' is copied internally.
// Returns the index of the string in the table.
int add_string(StrTable *st, char *s);

// Returns a pointer to the string stored at index 'i'.
char *get_string(StrTable *st, int i);

// Prints the given table to stdout.
void print_str_table(StrTable *st);

// Clears the allocated structure.
void free_str_table(StrTable *st);

// Variables Table
// ----------------------------------------------------------------------------

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
// This table only stores the variable name and type, and its declaration line.
struct var_table;
typedef struct var_table VarTable;

// Creates an empty variables table.
VarTable *create_var_table();

// Adds a fresh var to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_to_var_table(VarTable *vt, char *s, int line, Type type,
                     Type array_type, int scope, int dimension,
                     int relative_pos);

int add_var(VarTable *vt, char *s, int line, Type type, int scope,
            int relative_pos);

int add_array(VarTable *vt, char *s, int line, Type type, int scope,
              int dimension);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_for_create_var(VarTable *vt, char *s, int scope);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_var(VarTable *vt, char *s, int scope);

// Returns the variable name stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
char *get_name(VarTable *vt, int i);

// Returns the declaration line of the variable stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
int get_line(VarTable *vt, int i);

// Returns the variable type stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
Type get_type(VarTable *vt, int i);

Type get_array_type(VarTable *vt, int i);

// Returns the variable scope stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
int get_scope(VarTable *vt, int i);

int get_var_offset(VarTable *vt, int i);

// Prints the given table to stdout.
void print_var_table(char *name, VarTable *vt);

// Clears the allocated structure.
void free_var_table(VarTable *vt);

// Variables Table
// ----------------------------------------------------------------------------

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
// This table only stores the variable name and type, and its declaration line.
struct func_table;
typedef struct func_table FuncTable;

// Creates an empty variables table.
FuncTable *create_func_table();

int get_func_table_size(FuncTable *ft);

int get_func_num_vars(FuncTable *ft, int i);

void add_var_to_func(FuncTable *ft, int i);

// Adds a fresh var to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_func(FuncTable *vt, char *s, int line, Type type, int scope);
void add_func_params(FuncTable *vt, int i, Type *param_types, int num_param);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_for_create_func(FuncTable *vt, char *s, int scope);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_func(FuncTable *vt, char *s, int scope);

// Returns the variable name stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
char *get_func_name(FuncTable *vt, int i);

// Returns the declaration line of the variable stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
int get_func_line(FuncTable *vt, int i);

// Returns the variable type stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
Type get_func_type(FuncTable *vt, int i);

// Returns the variable scope stored at the given index.
// No check is made by this function, so make sure that the index is valid
// first.
int get_func_scope(FuncTable *vt, int i);

int get_func_num_params(FuncTable *vt, int i);

void set_func_ast_start(FuncTable *ft, int i, AST *ast);

AST *get_func_ast_start(FuncTable *ft, int i);

int get_func_is_builtin(FuncTable *ft, int i);

// Prints the given table to stdout.
void print_func_table(char *name, FuncTable *vt);

// Clears the allocated structure.
void free_func_table(FuncTable *vt);

#endif // TABLES_H
