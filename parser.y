// File name of generated parser.
%output "parser.c"
// Produces a ’parser.h’
%defines "parser.h"
// Give proper error messages when a syntax error is found.
%define parse.error verbose
// Enable lookahead correction to improve syntax error handling.
%define parse.lac full

%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "tables.h"
    int yylex(void);
    void yyerror(char const *s);
    void new_func();
    void check_new_func();
    int check_func();
    void new_var();
	void new_array();
    void check_new_var();
    int check_var();
	void type_error(Type type_left, Type type_right, char* op_str);
	Type check_type_sum(Type type_left, Type type_right, char* op_str);
    Type check_type_mul(Type type_left, Type type_right, char* op_str);
    Type check_type_op(Type type_left, Type type_right, char* op_str);
    Type check_type_assign(Type type_left, Type type_right, char* op_str);
	void check_condition(Type type, char* condition);
	void check_return(Type type_function, Type type_return);
	void check_array_position_type(Type type);
	void check_array_not_error(Type type);
	void check_function_num_params(int func_num_params, int call_num_params);
    extern int yylineno;
    extern char* yytext;
    extern char id_string[500];
    int scope = 0;
    int biggest_scope = 0;
    StrTable* str_table;
    VarTable* var_table;
    FuncTable* func_table;
	Type func_params[100];
	int func_num_params;
	int func_pos;
    Type type;
%}

%token COMMA SEMI

%token LPAR RPAR LBRA RBRA LCBRA RCBRA
%token LT GT EQ NOT AND OR PLUS MINUS TIMES OVER ASSIGN

%token	STRING INT FLOAT VOID
%token	STR_VAL INT_VAL FLOAT_VAL ID

%token	IF ELSE WHILE RETURN
%token UNKNOWN

%precedence RPAR
%precedence ELSE

%left OR
%left AND
%left EQ
%left LT GT
%left PLUS MINUS
%left TIMES OVER
%precedence NOT
%precedence UMINUS

%%
program
	: program_stmt
	| program program_stmt
	;

program_stmt
	: func_declaration
	| declare_id
	| declare_array
	;

stmt_list
	: %empty
	| stmt_list_mult
	;

stmt_list_mult
	: stmt
	| stmt_list_mult stmt
	;

stmt
	: loop_stmt
	| if_stmt
	| assign
	| declare_id
	| declare_array
	| func_call SEMI
	| RETURN expr SEMI {check_return(get_func_type(func_table, scope - 1), $2);}
	| RETURN SEMI {check_return(get_func_type(func_table, scope - 1), VOID_TYPE);}
	;

type
	: INT { type = INT_TYPE; }
	| FLOAT { type = REAL_TYPE; }
	| STRING { type = STR_TYPE; }
	| VOID { type = VOID_TYPE; }
	;

func_declaration
	: type ID { check_new_func(); new_func();
		biggest_scope++; scope = biggest_scope;
		func_num_params = 0;
		$$ = get_func_table_size(func_table)-1; } LPAR opt_param_type_list { add_func_params(func_table, $3, func_params, func_num_params);} RPAR LCBRA stmt_list RCBRA { scope = 0; }
	;

param_type
	: type ID { check_new_var(); new_var(); }
	;

param_type_list
	: param_type { func_params[func_num_params] = type; func_num_params++; }
	| param_type_list COMMA param_type { func_params[func_num_params] = type; func_num_params++; }

opt_param_type_list
	: %empty
	| param_type_list
	;

func_call
	: ID { func_pos = check_func(); func_num_params = 0;
		$$ = get_func_type(func_table, func_pos); } LPAR opt_arg_list {
		check_function_num_params(get_func_num_params(func_table, func_pos), func_num_params); } RPAR {$$ = $2;}
	;

arg_list
	: expr { func_params[func_num_params] = type; func_num_params++; }
	| arg_list COMMA expr { func_params[func_num_params] = type; func_num_params++; }
	;

opt_arg_list
	: %empty
	| arg_list
	;

loop_stmt
	: WHILE LPAR expr RPAR LCBRA stmt_list RCBRA {check_condition($3, "while");}
	;

if_stmt
	: IF LPAR expr RPAR LCBRA stmt_list RCBRA ELSE LCBRA stmt_list RCBRA {check_condition($3, "if");}
	| IF LPAR expr RPAR LCBRA stmt_list RCBRA {check_condition($3, "if");}
	;

array_base_declaration
	: type ID LBRA {
		check_new_var(); new_array();
	}
	;

declare_array
	: array_base_declaration expr RBRA SEMI
	| array_base_declaration INT_VAL RBRA ASSIGN LCBRA arg_list RCBRA SEMI
	;

declare_id
	: type ID { check_new_var(); new_var(); } SEMI
	| type ID { check_new_var(); new_var(); } ASSIGN expr SEMI {check_type_assign(type, $5, "=");}
	//| array_base_declaration expr RBRA SEMI
	//| array_base_declaration INT_VAL RBRA ASSIGN LCBRA arg_list RCBRA SEMI
	;

assign
	: ID { int pos = check_var(); $$ = get_type(var_table, pos); } ASSIGN expr SEMI {check_type_assign($2, $4, "=");}
	| ID { int pos = check_var();
		Type res_type = get_type(var_table, pos);
		if (res_type == ARRAY)
			res_type = get_array_type(var_table, pos);
		$$ = res_type; } LBRA expr RBRA ASSIGN expr SEMI {
		check_array_position_type($4);
		check_type_assign($2, $7, "="); }
	;

expr
	: LPAR expr RPAR {$$ = $2;}
	| NOT expr {$$ = check_type_op($2, $2, "!");}
	| MINUS expr %prec UMINUS {$$ = check_type_mul($2, $2, "-");}
	| expr AND expr {$$ = check_type_op($1, $3, "&&");}
	| expr OR expr {$$ = check_type_op($1, $3, "||");}
	| expr LT expr {$$ = check_type_op($1, $3, "<");}
	| expr GT expr {$$ = check_type_op($1, $3, ">");}
	| expr EQ expr {$$ = check_type_op($1, $3, "==");}
	| expr TIMES expr {$$ = check_type_mul($1, $3, "*");}
	| expr OVER expr {$$ = check_type_mul($1, $3, "/");}
	| expr PLUS expr {$$ = check_type_sum($1, $3, "+");}
	| expr MINUS expr {$$ = check_type_mul($1, $3, "-");}
	| ID { int pos = check_var();
		Type res_type = get_type(var_table, pos);
		if (res_type == ARRAY)
			res_type = get_array_type(var_table, pos);
		$$ = res_type; } LBRA expr RBRA { check_array_position_type($4); }
	| func_call {$$ = $1;}
	| INT_VAL {$$ = INT_TYPE;}
	| FLOAT_VAL {$$ = REAL_TYPE;}
	| STR_VAL {$$ = STR_TYPE;}
	| ID { check_var(); $$ = get_type(var_table, lookup_var(var_table, id_string, scope)); }
	;
%%

int main() {
    str_table = create_str_table();
    var_table = create_var_table();
    func_table = create_func_table();
    if (yyparse() == 0) printf("PARSE SUCCESSFUL!\n");
    else                printf("PARSE FAILED!\n");
    printf("\n\n");
    print_str_table(str_table);
    printf("\n\n");
    print_func_table("Func", func_table);
    printf("\n\n");
    print_var_table("Var", var_table);
    printf("\n\n");
    free_str_table(str_table);
    free_var_table(var_table);
    free_func_table(func_table);
    return 0;

}

void yyerror (char const *s) {
	printf("SYNTAX ERROR (%d): %s\n", yylineno, s);
	exit(EXIT_FAILURE);
}

void new_func() {
    add_func(func_table, id_string, yylineno, type, scope);
}

int check_func() {
	int pos = lookup_func(func_table, id_string, scope);
    if(pos == -1) { // variable is used but do not exist
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n", yylineno, id_string);
        exit(EXIT_FAILURE);
    }
	return pos;
}

void check_new_func() {
    int table_index = lookup_for_create_func(func_table, id_string, scope);
    if(table_index != -1) { // variable is declared but already exist
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n", yylineno, id_string, get_func_line(func_table, table_index));
        exit(EXIT_FAILURE);
    }
}

void new_var() {
    add_var(var_table, id_string, yylineno, type, scope);
}
void new_array() {
    add_array(var_table, id_string, yylineno, type, scope, 0);
}

int check_var() {
	int pos = lookup_var(var_table, id_string, scope);
    if(pos == -1) { // variable is used but do not exist
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n", yylineno, id_string);
        exit(EXIT_FAILURE);
    }
	return pos;
}

void check_new_var() {
    int table_index = lookup_for_create_var(var_table, id_string, scope);
    if(table_index != -1) { // variable is declared but already exist
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n", yylineno, id_string, get_line(var_table, table_index));
        exit(EXIT_FAILURE);
    }
}

void type_error(Type type_left, Type type_right, char* op_str) {
	print_var_table("var", var_table);
	printf("SEMANTIC ERROR (%d): incompatible types for operator '%s', LHS is '%s' and RHS is '%s'.\n", yylineno, op_str, get_text(type_left), get_text(type_right));
	exit(EXIT_FAILURE);
}

Type check_type_sum(Type type_left, Type type_right, char* op_str) {
    Type type = sum(type_left, type_right);
    if(type == ERROR) {
	    type_error(type_left, type_right, op_str);
	}
    return type;
}
Type check_type_mul(Type type_left, Type type_right, char* op_str) {
    Type type = mul(type_left, type_right);
    if(type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }   
    return type;
}
Type check_type_op(Type type_left, Type type_right, char* op_str) {
    Type type = op(type_left, type_right);
    if(type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }   
    return type;
}
Type check_type_assign(Type type_left, Type type_right, char* op_str) {
    Type type = assign(type_left, type_right);
    if(type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }   
    return type;
}

void check_condition(Type type, char* condition) {
    if(type != INT_TYPE) {
        printf("SEMANTIC ERROR (%d): conditional expression in '%s' is '%s' instead of 'integer'.\n", yylineno, condition, get_text(type));
        exit(EXIT_FAILURE);
    }   
}

void check_return(Type type_function, Type type_return) {
    Type type = assign(type_function, type_return);
    if(!(type_function == VOID_TYPE && type_return == VOID_TYPE) && type == ERROR) {
        printf("SEMANTIC ERROR (%d): return type is '%s' but function should return '%s'.\n", yylineno, get_text(type_return), get_text(type_function));
        exit(EXIT_FAILURE);
    }   
}

void check_array_position_type(Type type) {
	if(type != INT_TYPE) {
		printf("SEMANTIC ERROR (%d): array should access 'integer' position, but it was given '%s'.\n", yylineno, get_text(type));
		exit(EXIT_FAILURE);
	}
}

void check_array_not_error(Type type) {
	if(type == ERROR) {
		printf("SEMANTIC ERROR (%d): assign syntax should have a valid array, but it is type '%s'.\n", yylineno, get_text(type));
		exit(EXIT_FAILURE);
	}
}

void check_function_num_params(int func_num_params, int call_num_params) {
	if(func_num_params != call_num_params) {
		printf("SEMANTIC ERROR (%d): function call have '%d' arguments, but is expected to have '%d'.\n", yylineno, call_num_params, func_num_params);
		exit(EXIT_FAILURE);
	}
}
