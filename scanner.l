%{

#include <stdio.h>

void process_token(const char* tk) {
    printf("%d: %s -> %s\n", yylineno, yytext, tk);
}

%}

%option yylineno

int_val     			[0-9]+
float_val    			[0-9]+"."[0-9]+
str_val     			\"[^"\n]*\"

id          			[a-zA-Z_][0-9a-zA-Z_]*

ignore      			[ \t\n]+
single_line_comment 	"//".*\n

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
","         { process_token("COMMA"); }
";"         { process_token("SEMI"); }
"while" 	{ process_token("WHILE"); }
"if"        { process_token("IF"); }
"else"      { process_token("ELSE"); }
"return"    { process_token("RETURN"); }
"void"      { process_token("VOID"); }
"int"       { process_token("INT"); }
"float"     { process_token("FLOAT"); }
"char*"     { process_token("STRING"); }
"="         { process_token("ASSIGN"); }
"("         { process_token("LPAR"); }
")"         { process_token("RPAR"); }
"["         { process_token("LBRA"); }
"]"         { process_token("RBRA"); }
"{"         { process_token("LCBRA"); }
"}"         { process_token("RCBRA"); }
"=="        { process_token("EQ"); }
"<"         { process_token("LT"); }
">"         { process_token("GT"); }
"!"         { process_token("NOT"); }
"&&"        { process_token("AND"); }
"||"        { process_token("OR"); }
"+"         { process_token("PLUS"); }
"-"         { process_token("MINUS"); }
"*"         { process_token("TIMES"); }
"/"         { process_token("OVER"); }
{int_val}   { process_token("INT_VAL"); }
{float_val} { process_token("FLOAT_VAL"); }
{str_val}   { process_token("STR_VAL"); }

{id}        { process_token("ID"); }

            /* Be sure to keep this as the last rule */
.           { process_token("UNKNOWN"); }
%%

int main() {
	yylex();
	return 0;
}
