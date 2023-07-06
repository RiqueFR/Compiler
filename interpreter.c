
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "interpreter.h"
#include "tables.h"
#include "types.h"

// ----------------------------------------------------------------------------

extern StrTable *str_table;
extern VarTable *var_table;
extern FuncTable *func_table;

AST* main_ast = NULL;


typedef union {
    int   as_int;
    float as_float;
} Word;

#define FRAME_MAX_VARS 100

typedef frame {
	int function;
	Word variables[FRAME_MAX_VARS];
} Frame;

// ----------------------------------------------------------------------------

// Data stack -----------------------------------------------------------------

#define STACK_SIZE 100

Word stack[STACK_SIZE];
int sp; // stack pointer

// All these ops should have a boundary check, buuuut... X_X

void pushi(int x) {
    stack[++sp].as_int = x;
}

int popi() {
    return stack[sp--].as_int;
}

void pushf(float x) {
    stack[++sp].as_float = x;
}

float popf() {
    return stack[sp--].as_float;
}

void init_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        stack[i].as_int = 0;
    }
    sp = -1;
}

void print_stack() {
    printf("*** STACK: ");
    for (int i = 0; i <= sp; i++) {
        printf("%d ", stack[i].as_int);
    }
    printf("\n");
}

// ----------------------------------------------------------------------------

// Variables memory -----------------------------------------------------------

#define MEM_SIZE 100

Word mem[MEM_SIZE];

void storei(int addr, int val) {
    mem[addr].as_int = val;
}

int loadi(int addr) {
    return mem[addr].as_int;
}

void storef(int addr, float val) {
    mem[addr].as_float = val;
}

float loadf(int addr) {
    return mem[addr].as_float;
}

void init_mem() {
    for (int addr = 0; addr < MEM_SIZE; addr++) {
        mem[addr].as_int = 0;
    }
}

// ----------------------------------------------------------------------------

#define TRACE
#ifdef TRACE
#define trace(msg) printf("TRACE: %s\n", msg)
#else
#define trace(msg)
#endif

#define MAX_STR_SIZE 128
static char str_buf[MAX_STR_SIZE];
#define clear_str_buf() str_buf[0] = '\0'

void rec_run_ast(AST *ast);

void read_int(int var_idx) {
    int x;
    printf("read (int): ");
    scanf("%d", &x);
    storei(var_idx, x);
}

void read_real(int var_idx) {
    float x;
    printf("read (real): ");
    scanf("%f", &x);
    storef(var_idx, x);
}

void read_bool(int var_idx) {
    int x;
    do {
        printf("read (bool - 0 = false, 1 = true): ");
        scanf("%d", &x);
    } while (x != 0 && x != 1);
    storei(var_idx, x);
}

void read_str(int var_idx) {
    printf("read (str): ");
    clear_str_buf();
    scanf("%s", str_buf);   // Did anyone say Buffer Overflow..? ;P
    storei(var_idx, add_string(str_table, str_buf));
}

void write_int() {
    printf("%d\n", popi());
}

void write_real() {
    printf("%f\n", popf());
}

void write_bool() {
    popi() == 0 ? printf("false\n") : printf("true\n");
}

// Helper function to write strings.
void escape_str(const char* s, char *n) {
    int i = 0, j = 0;
    char c;
    while ((c = s[i++]) != '\0') {
        if (c == '"') { continue; }
        else if (c == '\\' && s[i] == 'n') {
            n[j++] = '\n';
            i++;
        } else {
            n[j++] = c;
        }
    }
    n[j] = '\0';
}

void write_str() {
    int s = popi(); // String pointer
    clear_str_buf();
    escape_str(get_string(str_table, s), str_buf);
    printf(str_buf); // Weird language semantics, if printing a string, no new line.
}

// ----------------------------------------------------------------------------

void run_and(AST* ast) {
	trace("and");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() && val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushi(popf() && val_float2);
			break;
		default:;
	}
}

void run_array_use(AST* ast) {}

void run_assign(AST *ast) {
	trace("assign");
	AST* expr_ast = get_child(ast, 1);
	Type expr_type = get_node_type(expr_ast);
    rec_run_ast(expr_ast); // run expr
    int idx = get_data(get_child(ast, 0)); // get data from VARUSE
	switch(expr_type) {
		case INT_TYPE:
			storei(idx, popi());
			break;
		case REAL_TYPE:
			storef(idx, popf());
			break;
		case STR_TYPE:
			storei(idx, popi());
			break;
		default:;
	}
}

void run_block(AST *ast) {
	trace("block");
	for(int i = 0; i < get_child_count(ast); i++) {
		rec_run_ast(get_child(ast, i));
	}
}

void run_eq(AST *ast) {
	trace("eq");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			pushi(popi() == popi());
			break;
		case REAL_TYPE:
			pushi(popf() == popf());
			break;
		default:;
	}
}

void run_func_decl(AST* ast) {
	trace("func_decl");
	int func_idx = get_data(ast);
	if (!strcmp(get_func_name(func_table, func_idx), "main")) main_ast = ast;
	set_func_ast_start(func_table, func_idx, ast);
}

void run_printf(AST *ast) {
	trace("print");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			write_int();
			break;
		case REAL_TYPE:
			write_real();
			break;
		case STR_TYPE:
			write_str();
			break;
		default:;
	}
}

void run_func_use(AST* ast) {
	trace("func_use");
	// run called function
	int data = get_data(ast);
	if(get_func_is_builtin(func_table, data)) {
		if(!strcmp(get_func_name(func_table, data), "printf"))
			run_printf(ast);
	} else {
		rec_run_ast(get_child(get_func_ast_start(func_table, data), 1));
	}
}

void run_if(AST *ast) {
	trace("if");
	rec_run_ast(get_child(ast, 0));
	if(popi()) {
		rec_run_ast(get_child(ast, 1));
	} else {
		if(get_child_count(ast) > 2)
			rec_run_ast(get_child(ast, 2));
	}
}

void run_int_val(AST *ast) {
	trace("int_val");
	pushi(get_data(ast));
}

void run_gt(AST *ast) {
	trace("gt");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() > val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushi(popf() > val_float2);
			break;
		default:;
	}
}

void run_lt(AST *ast) {
	trace("lt");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() < val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushi(popf() < val_float2);
			break;
		default:;
	}
}

void run_minus(AST *ast) {
	trace("minus");
	rec_run_ast(get_child(ast, 0));
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() - val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushf(popf() - val_float2);
			break;
		default:;
	}
}

void run_neg(AST* ast) {
	trace("neg");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			pushi(-popi());
			break;
		case REAL_TYPE:
			pushi(-popf());
			break;
		default:;
	}
}

void run_not(AST* ast) {
	trace("not");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			pushi(!popi());
			break;
		case REAL_TYPE:
			pushi(!popf());
			break;
		default:;
	}
}

void run_or(AST* ast) {
	trace("or");
	AST* expr_ast = get_child(ast, 0);
	rec_run_ast(expr_ast);
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() || val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushi(popf() || val_float2);
			break;
		default:;
	}
}

void run_over(AST *ast) {
	trace("over");
	rec_run_ast(get_child(ast, 0));
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(ast)) {
		case INT_TYPE:
			int val_int2 = popi();
			pushi(popi() / val_int2);
			break;
		case REAL_TYPE:
			float val_float2 = popf();
			pushf(popf() / val_float2);
			break;
		default:;
	}
}

void run_plus(AST *ast) {
	trace("plus");
	rec_run_ast(get_child(ast, 0));
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(ast)) {
		case INT_TYPE:
			pushi(popi() + popi());
			break;
		case REAL_TYPE:
			pushf(popf() + popf());
			break;
		default:;
	}
}

void run_program(AST *ast) {
    trace("program");
	rec_run_ast(get_child(ast, 0)); // run block
	// run main block node
	trace("main");
	rec_run_ast(get_child(main_ast, 1));
}

void run_read(AST *ast) {
	trace("read");
	AST* expr_ast = get_child(ast, 0);
	int idx = get_data(expr_ast);
	switch(get_node_type(expr_ast)) {
		case STR_TYPE:
			read_str(idx);
			break;
		case INT_TYPE:
			read_int(idx);
			break;
		case REAL_TYPE:
			read_real(idx);
			break;
		default:;
	}
}

void run_real_val(AST *ast) {
	trace("real_val");
	pushf(get_float_data(ast));
}

void run_return(AST* ast) {
	trace("return");
	if(get_child_count(ast) > 0)
		rec_run_ast(get_child(ast, 0));
}

void run_while(AST *ast) {
	trace("while");
	AST* condition_ast = get_child(ast, 0);
	rec_run_ast(condition_ast);
	while(popi()) {
		rec_run_ast(get_child(ast, 1));
		rec_run_ast(condition_ast);
	} 
}

void run_str_val(AST *ast) {
    trace("str_val");
    pushi(get_data(ast));
}

void run_times(AST *ast) {
	trace("times");
	rec_run_ast(get_child(ast, 0));
	rec_run_ast(get_child(ast, 1));
	switch(get_node_type(ast)) {
		case INT_TYPE:
			pushi(popi() * popi());
			break;
		case REAL_TYPE:
			pushf(popf() * popf());
			break;
		default:;
	}
}

void run_var_decl(AST *ast) {
    trace("var_decl");
    // Nothing to do, memory was already cleared upon initialization.
}

void run_var_list(AST *ast) {
    trace("var_list");
    // Nothing to do, memory was already cleared upon initialization.
}

void run_var_use(AST *ast) {
	trace("var_use");
	int idx = get_data(ast);
	if(get_type(var_table, idx) == REAL_TYPE)
		pushf(loadf(idx));
	else
		pushi(loadi(idx));
}

void run_void_val(AST* ast) {
	trace("void_val");
	// Do nothing
}

void run_i2r(AST* ast) {
    rec_run_ast(get_child(ast, 0));
	pushf((float)popi());
}

void run_r2i(AST* ast) {
    rec_run_ast(get_child(ast, 0));
	pushi((int)popf());
}

void rec_run_ast(AST *ast) {
    switch(get_kind(ast)) {
        case AND_NODE:   		run_and(ast); 		break;
        case ARRAY_USE_NODE:   	run_array_use(ast); break;
        case ASSIGN_NODE:   	run_assign(ast); 	break;
        case BLOCK_NODE:    	run_block(ast); 	break;
        case EQ_NODE:       	run_eq(ast); 		break;
        case FLOAT_VAL_NODE: 	run_real_val(ast); 	break;
        case FUNC_DECL_NODE: 	run_func_decl(ast); break;
        case FUNC_USE_NODE: 	run_func_use(ast); 	break;
        case IF_NODE: 			run_if(ast); 		break;
        case INT_VAL_NODE: 		run_int_val(ast); 	break;
        case GT_NODE: 			run_gt(ast); 		break;
        case LT_NODE: 			run_lt(ast); 		break;
        case MINUS_NODE: 		run_minus(ast); 	break;
		case NEG_NODE: 			run_neg(ast); 		break;
		case NOT_NODE: 			run_not(ast); 		break;
        case OR_NODE: 			run_or(ast); 		break;
        case OVER_NODE: 		run_over(ast); 		break;
        case PLUS_NODE: 		run_plus(ast); 		break;
        case PROGRAM_NODE: 		run_program(ast); 	break;
        /*case READ_NODE: 		run_read(ast); 		break;*/
        case RETURN_NODE: 		run_return(ast); 	break;
        case STRING_VAL_NODE: 	run_str_val(ast); 	break;
        case TIMES_NODE: 		run_times(ast); 	break;
        case VAR_DECL_NODE: 	run_var_decl(ast); 	break;
        case VAR_LIST_NODE: 	run_var_list(ast); 	break;
        case VAR_USE_NODE: 		run_var_use(ast); 	break;
        case VOID_VAL_NODE: 	run_void_val(ast); 	break;
        case WHILE_NODE: 		run_while(ast); 	break;
        /*case PRINTF_NODE: 		run_printf(ast); 	break;*/

        case I2R_NODE: 			run_i2r(ast); 		break;
        case R2I_NODE: 			run_r2i(ast); 		break;

        default:
            fprintf(stderr, "Invalid kind: %s!\n", kind2str(get_kind(ast)));
            exit(EXIT_FAILURE);
    }
}

// ----------------------------------------------------------------------------

void run_ast(AST *ast) {
    init_stack();
    init_mem();
    rec_run_ast(ast);
}
