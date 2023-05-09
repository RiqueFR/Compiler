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
    void new_var();
    void check_new_var();
    void check_var();
    void check_var_with_string();
    extern int yylineno;
    extern char* yytext;
    extern char id_string[500];
    int scope = 0;
    int biggest_scope = 0;
    StrTable* str_table;
    VarTable* var_table;
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
	| func_call SEMI
	| RETURN expr SEMI
	;

type
	: INT { type = INT_TYPE; }
	| FLOAT { type = REAL_TYPE; }
	| STRING { type = STR_TYPE; }
	| VOID { type = VOID_TYPE; }
	;

func_declaration
	: type ID { check_new_var(); } LPAR { biggest_scope++; scope = biggest_scope; } opt_param_type_list RPAR LCBRA stmt_list RCBRA { scope = 0; }
	;

param_type
	: type ID { check_new_var(); }
	;

param_type_list
	: param_type
	| param_type_list COMMA param_type

opt_param_type_list
	: %empty
	| param_type_list
	;

func_call
	: ID { check_var(); } LPAR opt_arg_list RPAR
	;

arg_list
	: expr
	| arg_list COMMA expr
	;

opt_arg_list
	: %empty
	| arg_list
	;

loop_stmt
	: WHILE LPAR expr RPAR LCBRA stmt_list RCBRA
	;

if_stmt
	: IF LPAR expr RPAR LCBRA stmt_list RCBRA ELSE LCBRA stmt_list RCBRA
	| IF LPAR expr RPAR LCBRA stmt_list RCBRA
	;

declare_id
	: type ID { check_new_var(); } SEMI
	| type ID { check_new_var(); } ASSIGN expr SEMI
	| type ID { check_new_var(); } LBRA INT_VAL RBRA ASSIGN LCBRA arg_list RCBRA SEMI
	;

assign
	: ID { check_var(); } ASSIGN expr SEMI
	| ID { check_var(); } LBRA INT_VAL RBRA ASSIGN expr SEMI
	;

expr
	: LPAR expr RPAR
	| NOT expr
	| MINUS expr %prec UMINUS
	| expr AND expr
	| expr OR expr
	| expr LT expr
	| expr GT expr
	| expr EQ expr
	| expr TIMES expr
	| expr OVER expr
	| expr PLUS expr
	| expr MINUS expr
	| ID { check_var(); } LBRA expr RBRA
	| func_call
	| INT_VAL
	| FLOAT_VAL
	| STR_VAL
	| ID { check_var(); }
	;
%%

int main() {
    str_table = create_str_table();
    var_table = create_var_table();
    if (yyparse() == 0) printf("PARSE SUCCESSFUL!\n");
    else                printf("PARSE FAILED!\n");
    printf("\n\n");
    print_str_table(str_table);
    printf("\n\n");
    print_var_table(var_table);
    printf("\n\n");
    free_str_table(str_table);
    free_var_table(var_table);
    return 0;

}

void yyerror (char const *s) {
	printf("SYNTAX ERROR (%d): %s\n", yylineno, s);
	exit(EXIT_FAILURE);
}

void new_var() {
    add_var(var_table, id_string, yylineno, type, scope);
}

void check_var() {
    if(lookup_var(var_table, id_string, scope) == -1) { // variable is used but do not exist
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n", yylineno, id_string);
        exit(EXIT_FAILURE);
    }
}
void check_var_with_string() {
    if(lookup_var(var_table, id_string, scope) == -1) { // variable is used but do not exist
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n", yylineno, id_string);
        exit(EXIT_FAILURE);
    }
}

void check_new_var() {
    int table_index = lookup_var(var_table, id_string, scope);
    if(table_index != -1) { // variable is declared but already exist
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n", yylineno, id_string, get_line(var_table, table_index));
        exit(EXIT_FAILURE);
    }
    new_var();
}
