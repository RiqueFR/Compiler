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
    int yylex(void);
    void yyerror(char const *s);
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
	: func_declaration
	| assign
	;

stmt
	: %empty
	| loop_stmt
	| if_stmt
	| assign
	| func_call SEMI
	;

type
	: INT
	| FLOAT
	| STRING
	| VOID
	;

func_declaration
	: type ID LPAR opt_param_type_list RPAR LCBRA stmt RCBRA
	;

param_type
	: type ID
	;

param_type_list
	: param_type
	| param_type_list COMMA param_type

opt_param_type_list
	: %empty
	| param_type_list
	;

func_call
	: ID LPAR opt_arg_list RPAR
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
	: WHILE LPAR expr RPAR LCBRA stmt RCBRA
	;

if_stmt
	: IF LPAR expr RPAR LCBRA stmt RCBRA ELSE LCBRA stmt RCBRA
	| IF LPAR expr RPAR LCBRA stmt RCBRA
	;

assign
	: type ID ASSIGN expr SEMI
	| type ID LBRA INT_VAL RBRA ASSIGN LCBRA arg_list RCBRA SEMI
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
	| ID LBRA expr RBRA
	| func_call
	| INT_VAL
	| FLOAT_VAL
	| STR_VAL
	| ID
	;
%%
