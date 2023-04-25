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

%left OR
%left AND
%left EQ
%left LT GT
%left PLUS MINUS
%left TIMES OVER
%precedence NOT
%precedence UMINUS

%%
assign
	: ID ASSIGN expr
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
	| INT_VAL
	| FLOAT_VAL
	| STR_VAL
	| ID
	;
%%
