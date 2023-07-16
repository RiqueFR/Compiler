%option outfile="scanner.c"
%option noyywrap
%option nounput
%option noinput
%option yylineno

%{
    #include <string.h>
    #include "ast.h"
    #include "parser.h" // For the token types from bison.
    #include "tables.h"
    extern StrTable* str_table;
    char id_string[500];
    int new_or_get_string();
%}

int_val     			[0-9]+
float_val    			[0-9]+"."[0-9]+
str_val     			\"[^"\n]*\"

id          			[a-zA-Z_][0-9a-zA-Z_]*

ignore      			[ \t\n]+
single_line_comment 		"//".*\n

%x IN_COMMENT

%%
<INITIAL>{
"/*"      BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        // ear new line
}
{ignore}    			{ }
{single_line_comment}  { }
","         { return COMMA; }
";"         { return SEMI; }
"while"     { return WHILE; }
"if"        { return IF; }
"else"      { return ELSE; }
"return"    { return RETURN; }
"void"      { return VOID; }
"int"       { return INT; }
"float"     { return FLOAT; }
"char*"     { return STRING; }
"="         { return ASSIGN; }
"("         { return LPAR; }
")"         { return RPAR; }
"["         { return LBRA; }
"]"         { return RBRA; }
"{"         { return LCBRA; }
"}"         { return RCBRA; }
"=="        { return EQ; }
"<"         { return LT; }
">"         { return GT; }
"!"         { return NOT; }
"&&"        { return AND; }
"||"        { return OR; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return OVER; }
{int_val}   { return INT_VAL; }
{float_val} { return FLOAT_VAL; }
{str_val}   { new_or_get_string(); return STR_VAL; }

{id}        { strcpy(id_string, yytext); return ID; }

            /* Be sure to keep this as the last rule */
.           { return UNKNOWN; }
%%

int new_or_get_string() {
	// remove "" from string
	char aux[500];
	int s_size = strlen(yytext);
	for(int i = 0; i < s_size-1; i++) {
		aux[i] = yytext[i+1];
	}
	aux[s_size-2] = '\0';
	return add_string(str_table, aux);
}
