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
    #include "ast.h"
	#include "tables.h"
	#include "interpreter.h"

    int yylex(void);
	int yylex_destroy(void);
    void yyerror(char const *s);
    int new_func();
    void check_new_func();
    int check_func();
    int new_var();
	int new_array(int size);
    void check_new_var();
    int check_var();
	void type_error(Type type_left, Type type_right, const char* op_str);
	Type check_type_sum(AST* type_left, AST* type_right, char* op_str);
    Type check_type_mul(AST* type_left, AST* type_right, char* op_str);
    Type check_type_op(AST* type_left, AST* type_right, char* op_str);
    Type check_type_assign(AST* type_left, AST* type_right, char* op_str);
	AST* unify_bin_return(Type func_type, AST* return_exp_node, NodeKind kind, Unif (*unify)(Type,Type));
	AST* unify_bin_node(AST* l, AST* r,
                    NodeKind kind, const char* op, Unif (*unify)(Type,Type));
	void check_condition(AST* type, char* condition);
	void check_return(Type type_function, AST* ast_return);
	void check_array_position_type(AST* type);
	void check_array_not_error(AST* type);
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
	AST* program;
	AST* func_call_node;
%}

%define api.value.type {AST*}

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
base
	: program { program = new_subtree(PROGRAM_NODE, VOID_TYPE, 1, $1); }
	;

program
	: program_stmt { $$ = new_subtree(BLOCK_NODE, VOID_TYPE, 1, $1); }
	| program program_stmt { add_child($1, $2); $$ = $1; }
	;

program_stmt
	: func_declaration { $$ = $1; }
	| declare_id { $$ = $1; }
	| declare_array { $$ = $1; }
	;

stmt_list
	: %empty { $$ = new_subtree(BLOCK_NODE, VOID_TYPE, 0); }
	| stmt_list_mult { $$ = $1; }
	;

stmt_list_mult
	: stmt { $$ = new_subtree(BLOCK_NODE, VOID_TYPE, 1, $1); }
	| stmt_list_mult stmt { add_child($1, $2); $$ = $1; }
	;

stmt
	: loop_stmt { $$ = $1; }
	| if_stmt { $$ = $1; }
	| assign { $$ = $1; }
	| declare_id { $$ = $1; }
	| declare_array { $$ = $1; }
	| func_call SEMI { $$ = $1; }
	| RETURN expr SEMI { Type loc_func_type = get_func_type(func_table, scope); check_return(loc_func_type, $2); $$ = unify_bin_return(loc_func_type, $2, RETURN_NODE, assign); }//$$ = new_subtree(RETURN_NODE, VOID_TYPE, 1, $2); }
	| RETURN SEMI { check_return(get_func_type(func_table, scope), new_node(VOID_VAL_NODE, 0, VOID_TYPE)); $$ = new_node(RETURN_NODE, 0, VOID_TYPE); }
	;

type
	: INT { type = INT_TYPE; }
	| FLOAT { type = REAL_TYPE; }
	| STRING { type = STR_TYPE; }
	| VOID { type = VOID_TYPE; }
	;

func_declaration
	: type ID { check_new_func(); int pos = new_func();
		// scope is func position + 1, scope 0 is global scope
		scope = pos + 1;
		func_num_params = 0;
		$1 = new_node(FUNC_DECL_NODE, pos, get_func_type(func_table, pos));
		} LPAR opt_param_type_list {
		add_func_params(func_table, get_data($1), func_params, func_num_params);
		} RPAR LCBRA stmt_list RCBRA { scope = 0; add_child($1, $5); add_child($1, $9); $$ = $1; }
	;

param_type
	: type ID { check_new_var(); int pos = add_var(var_table, id_string, yylineno, type, scope, -func_num_params); $$ = new_node(VAR_DECL_NODE, pos, get_type(var_table, pos)); }
	;

param_type_list
	: param_type { func_params[func_num_params] = type; func_num_params++; $$ = new_subtree(VAR_LIST_NODE, VOID_TYPE, 1, $1); }
	| param_type_list COMMA param_type { func_params[func_num_params] = type; func_num_params++; add_child($1, $3); $$ = $1; }

opt_param_type_list
	: %empty { $$ = new_subtree(VAR_LIST_NODE, VOID_TYPE, 0); }
	| param_type_list { $$ = $1; }
	;

func_call
	: ID { func_pos = check_func(); func_num_params = 0;
		func_call_node = new_node(FUNC_USE_NODE, func_pos, get_func_type(func_table, func_pos)); $1 = func_call_node; } LPAR opt_arg_list {
		check_function_num_params(get_func_num_params(func_table, func_pos), func_num_params); } RPAR {$$ = $1;}
	;

arg_list
	: expr { func_params[func_num_params] = type; func_num_params++; add_child(func_call_node, $1); }
	| arg_list COMMA expr { func_params[func_num_params] = type; func_num_params++; add_child(func_call_node, $3); }
	;

opt_arg_list
	: %empty
	| arg_list
	;

loop_stmt
	: WHILE LPAR expr RPAR LCBRA stmt_list RCBRA { check_condition($3, "while"); $$ = new_subtree(WHILE_NODE, VOID_TYPE, 2, $3, $6); }
	;

if_stmt
	: IF LPAR expr RPAR LCBRA stmt_list RCBRA ELSE LCBRA stmt_list RCBRA { check_condition($3, "if"); $$ = new_subtree(IF_NODE, VOID_TYPE, 3, $3, $6, $10); }
	| IF LPAR expr RPAR LCBRA stmt_list RCBRA { check_condition($3, "if"); $$ = new_subtree(IF_NODE, VOID_TYPE, 2, $3, $6); }
	;

array_base_declaration
	: type ID LBRA INT_VAL {
		check_new_var(); int pos = new_array(atoi(yytext)); $1 = new_node(VAR_DECL_NODE, pos, get_type(var_table, pos));
	} RBRA { $$ = $1; }
	;

declare_array
	: array_base_declaration SEMI { $$ = $1; }
	| array_base_declaration ASSIGN LCBRA arg_list RCBRA SEMI { $$ = $1; }
	;

declare_id
	: type ID { check_new_var(); int pos = new_var(); $1 = new_node(VAR_DECL_NODE, pos, get_type(var_table, pos)); } SEMI { $$ = $1; }
	| type ID { check_new_var(); int pos = new_var(); $1 = new_node(VAR_DECL_NODE, pos, get_type(var_table, pos)); } ASSIGN expr SEMI {
		$$ = unify_bin_node($1, $5, ASSIGN_NODE, "<", assign); }
	;

assign
	: ID { int pos = check_var(); $1 = new_node(VAR_USE_NODE, pos, get_type(var_table, pos)); } ASSIGN expr SEMI { $$ = unify_bin_node($1, $4, ASSIGN_NODE, "<", assign); }
	| ID { int pos = check_var();
		Type res_type = get_type(var_table, pos);
		if (res_type == ARRAY)
			res_type = get_array_type(var_table, pos);
		$1 = new_node(VAR_USE_NODE, pos, res_type); } LBRA expr { AST* node = new_node(ARRAY_USE_NODE, get_data($1), ARRAY); add_child(node, $1); add_child(node, $4); $3 = node; } RBRA ASSIGN expr SEMI {
		check_array_position_type($4);
		check_type_assign($1, $8, "=");
		$$ = new_subtree(ASSIGN_NODE, VOID_TYPE, 2, $3, $8);
		}
	;

expr
	: LPAR expr RPAR { $$ = $2; }
	| NOT expr { $$ = new_subtree(NOT_NODE, check_type_op($2, $2, "!"), 1, $2); }
	| MINUS expr %prec UMINUS { $$ = new_subtree(NEG_NODE, check_type_mul($2, $2, "-"), 1, $2); }
	| expr AND expr { $$ = unify_bin_node($1, $3, AND_NODE, "&&", op); }
	| expr OR expr { $$ = unify_bin_node($1, $3, OR_NODE, "||", op); }
	| expr LT expr { $$ = unify_bin_node($1, $3, LT_NODE, "<", op); }
	| expr GT expr { $$ = unify_bin_node($1, $3, GT_NODE, ">", op); }
	| expr EQ expr { $$ = unify_bin_node($1, $3, EQ_NODE, "==", op); }
	| expr TIMES expr { $$ = unify_bin_node($1, $3, TIMES_NODE, "*", mul); }
	| expr OVER expr { $$ = unify_bin_node($1, $3, OVER_NODE, "<", mul); }
	| expr PLUS expr { $$ = unify_bin_node($1, $3, PLUS_NODE, "<", sum); }
	| expr MINUS expr { $$ = unify_bin_node($1, $3, MINUS_NODE, "<", mul); }
	| ID {
	    int pos = check_var();
		$1 = new_node(VAR_USE_NODE, pos, ARRAY); }
	  LBRA expr RBRA {
		int pos = get_data($1);
		Type res_type = get_type(var_table, pos);
		if (res_type == ARRAY)
			res_type = get_array_type(var_table, pos);
		check_array_position_type($4); $$ = new_subtree(ARRAY_USE_NODE, res_type, 2, $1, $4);
		}
	| func_call { $$ = $1; }
	| INT_VAL { $$ = new_node(INT_VAL_NODE, atoi(yytext), INT_TYPE); }
	| FLOAT_VAL { AST* node = new_node(FLOAT_VAL_NODE, 0, REAL_TYPE); set_float_data(node, atof(yytext)); $$ = node; }
	| STR_VAL { $$ = new_node(STRING_VAL_NODE, add_string(str_table, yytext), STR_TYPE); }
	| ID { int pos = check_var(); $$ = new_node(VAR_USE_NODE, pos, get_type(var_table, pos)); }
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
	print_dot(program);

	stdin = fopen(ctermid(NULL), "r");
	run_ast(program);
	fclose(stdin);

    free_str_table(str_table);
    free_var_table(var_table);
    free_func_table(func_table);
	free_tree(program);
	yylex_destroy();
    return 0;

}

void yyerror (char const *s) {
	printf("SYNTAX ERROR (%d): %s\n", yylineno, s);
	exit(EXIT_FAILURE);
}

int new_func() {
    return add_func(func_table, id_string, yylineno, type, scope);
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

int new_var() {
	int relative_pos = get_func_num_vars(func_table, scope);
	add_var_to_func(func_table, scope);
    return add_var(var_table, id_string, yylineno, type, scope, relative_pos);
}
int new_array(int size) {
	int relative_pos = get_func_num_vars(func_table, scope);
	add_array_to_func(func_table, scope, size);
    return add_array(var_table, id_string, yylineno, type, scope, 0, relative_pos, size);
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

void type_error(Type type_left, Type type_right, const char* op_str) {
	print_var_table("var", var_table);
	printf("SEMANTIC ERROR (%d): incompatible types for operator '%s', LHS is '%s' and RHS is '%s'.\n", yylineno, op_str, get_text(type_left), get_text(type_right));
	exit(EXIT_FAILURE);
}

Type check_type_sum(AST* ast_left, AST* ast_right, char* op_str) {
	Type type_left = get_node_type(ast_left);
	Type type_right = get_node_type(ast_right);
    Unif unif = sum(type_left, type_right);
    if(unif.type == ERROR) {
	    type_error(type_left, type_right, op_str);
	}
    return unif.type;
}
Type check_type_mul(AST* ast_left, AST* ast_right, char* op_str) {
	Type type_left = get_node_type(ast_left);
	Type type_right = get_node_type(ast_right);
    Unif unif = mul(type_left, type_right);
    if(unif.type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }
    return unif.type;
}
Type check_type_op(AST* ast_left, AST* ast_right, char* op_str) {
	Type type_left = get_node_type(ast_left);
	Type type_right = get_node_type(ast_right);
    Unif unif = op(type_left, type_right);
    if(unif.type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }   
    return unif.type;
}
Type check_type_assign(AST* ast_left, AST* ast_right, char* op_str) {
	Type type_left = get_node_type(ast_left);
	Type type_right = get_node_type(ast_right);
    Unif unif = assign(type_left, type_right);
    if(unif.type == ERROR) {
	    type_error(type_left, type_right, op_str);
    }   
    return unif.type;
}

AST* create_conv_node(Conv conv, AST *n) {
    switch(conv) {
        case I2F:  return new_subtree(I2R_NODE, REAL_TYPE, 1, n);
        case F2I:  return new_subtree(R2I_NODE, INT_TYPE,  1, n);
        case NONE: return n;
        default:
            printf("INTERNAL ERROR: invalid conversion of types!\n");
            exit(EXIT_FAILURE);
    }
}

AST* unify_bin_return(Type func_type, AST* return_exp_node, NodeKind kind, Unif (*unify)(Type,Type)) {
    Type return_type = get_node_type(return_exp_node);
    Unif unif = unify(func_type, return_type);
	// check should already had been done
    return_exp_node = create_conv_node(unif.rc, return_exp_node);
    return new_subtree(kind, unif.type, 1, return_exp_node);
}

AST* unify_bin_node(AST* l, AST* r,
                    NodeKind kind, const char* op, Unif (*unify)(Type,Type)) {
    Type lt = get_node_type(l);
    Type rt = get_node_type(r);
    Unif unif = unify(lt, rt);
    if (unif.type == ERROR) {
        type_error(lt, rt, op);
    }
    l = create_conv_node(unif.lc, l);
    r = create_conv_node(unif.rc, r);
    return new_subtree(kind, unif.type, 2, l, r);
}

void check_condition(AST* ast, char* condition) {
	Type type = get_node_type(ast);
    if(type != INT_TYPE) {
        printf("SEMANTIC ERROR (%d): conditional expression in '%s' is '%s' instead of 'integer'.\n", yylineno, condition, get_text(type));
        exit(EXIT_FAILURE);
    }   
}

void check_return(Type type_function, AST* ast_return) {
	Type type_return = get_node_type(ast_return);
    Unif unif = assign(type_function, type_return);
    if(!(type_function == VOID_TYPE && type_return == VOID_TYPE) && unif.type == ERROR) {
        printf("SEMANTIC ERROR (%d): return type is '%s' but function should return '%s'.\n", yylineno, get_text(type_return), get_text(type_function));
        exit(EXIT_FAILURE);
    }   
}

void check_array_position_type(AST* ast) {
	Type type = get_node_type(ast);
	if(type != INT_TYPE) {
		printf("SEMANTIC ERROR (%d): array should access 'integer' position, but it was given '%s'.\n", yylineno, get_text(type));
		exit(EXIT_FAILURE);
	}
}

void check_array_not_error(AST* ast) {
	Type type = get_node_type(ast);
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
