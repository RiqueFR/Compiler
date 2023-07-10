
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

int ret_abort = 0;

AST* main_ast = NULL;

typedef union {
    int   as_int;
    float as_float;
} Word;

// ----------------------------------------------------------------------------

// Data stack -----------------------------------------------------------------

#define STACK_SIZE 100

Word stack[STACK_SIZE];
int sp; // stack pointer
int fp; // frame pointer
int cl; // control link

Word return_val;

// All these ops should have a boundary check, buuuut... X_X

void pushi(int val) {
    stack[++sp].as_int = val;
}

int popi() {
    return stack[sp--].as_int;
}

void pushf(float val) {
    stack[++sp].as_float = val;
}

float popf() {
    return stack[sp--].as_float;
}

int get_parami(int pos) {
    return stack[fp - pos - 2].as_int;
}
float get_paramf(int pos) {
    return stack[fp - pos - 2].as_float;
}

void init_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        stack[i].as_int = 0;
    }
    sp = -1;
	fp = 0;
	cl = 0;
}

void print_stack() {
    printf("*** STACK: ");
    for (int i = 0; i <= sp; i++) {
        printf("%d ", stack[i].as_int);
    }
    printf(" | sp: %d | fp: %d | cl: %d\n", sp, fp, cl);
}

void call_function(int func_pos) {
	pushi(cl);
	cl = fp;
	fp = ++sp;
	int num_vars = get_func_num_vars(func_table, func_pos);
	int num_params = get_func_num_params(func_table, func_pos);
	if(num_vars > num_params)
		sp += num_vars - num_params;
}

void return_function() {
	sp = fp - 1;
	fp = cl;
	cl = popi();
}

// ----------------------------------------------------------------------------

// Variables memory -----------------------------------------------------------

#define MEM_SIZE 100

Word mem[MEM_SIZE];

void storei(int addr, int val) {
	int offset = get_var_offset(var_table, addr);
	/*printf("fp: %d | pushoffset: %d | val: %d\n", fp, offset, val);*/
    stack[fp + offset].as_int = val;
}

int loadi(int addr) {
	int offset = get_var_offset(var_table, addr);
	/*printf("fp: %d | loadoffset: %d | val: %d\n", fp, offset, stack[fp + offset].as_int);*/
    return stack[fp + offset].as_int;
}

void storef(int addr, float val) {
	int offset = get_var_offset(var_table, addr);
    stack[fp + offset].as_float = val;
}

float loadf(int addr) {
	int offset = get_var_offset(var_table, addr);
    return stack[fp + offset].as_float;
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
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			pushi(get_parami(0));
			write_int();
			break;
		case REAL_TYPE:
			pushf(get_paramf(0));
			write_real();
			break;
		case STR_TYPE:
			pushi(get_parami(0));
			write_str();
			break;
		default:;
	}
}

void run_scanf(AST *ast) {
	trace("scan");
	AST* expr_ast = get_child(ast, 0);
	/*int pos = get_parami(0);*/
	pushi(get_data(expr_ast));
	int pos = popi();
	printf("scan -> %d\n", pos);
	pushi(pos);
	switch(get_node_type(expr_ast)) {
		case INT_TYPE:
			read_int(popi());
			break;
		case REAL_TYPE:
			read_real(popi());
			break;
		case STR_TYPE:
			read_str(popi());
			break;
		default:;
	}
}

void run_func_use(AST* ast) {
	trace("func_use");
	// run called function
	int data = get_data(ast);
	printf("data: %d\n", data);
	for(int i = get_child_count(ast) - 1; i >=0; i--) {
		rec_run_ast(get_child(ast, i));
	}
	/*printf("in ");*/
	/*print_stack();*/
	if(get_func_is_builtin(func_table, data)) {
		if(!strcmp(get_func_name(func_table, data), "printf")) {
			call_function(data);
			run_printf(ast);
			return_function();
		} else if(!strcmp(get_func_name(func_table, data), "scanf")) {
			run_scanf(ast);
			popi();	
		}
	} else {
		call_function(data);
		rec_run_ast(get_child(get_func_ast_start(func_table, data), 0));
		rec_run_ast(get_child(get_func_ast_start(func_table, data), 1));
		return_function();
	}
	for(int i = 0; i < get_func_num_params(func_table, data); i++) {
		popi();
	}
	if(get_node_type(ast) != VOID_TYPE) {
		// get return from function
		switch(get_node_type(ast)) {
			case INT_TYPE:
				pushi(return_val.as_int);
				break;
			case REAL_TYPE:
				pushi(return_val.as_float);
				break;
			case STR_TYPE:
				pushi(return_val.as_int);
				break;
			default:;
		}
	}
	/*printf("out ");*/
	/*print_stack();*/
	ret_abort = 0;
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
	/*run_func_use(get_child(main_ast, 1));*/
	call_function(get_data(main_ast));
	rec_run_ast(get_child(main_ast, 1));
	return_function();
	print_stack();
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
	AST* child = get_child(ast, 0);
	if(get_child_count(ast) > 0) {
		rec_run_ast(child);
	}
	ret_abort = 1;
	switch(get_node_type(child)) {
		case INT_TYPE:
			return_val.as_int = popi();
			break;
		case REAL_TYPE:
			return_val.as_float = popf();
			break;
		case STR_TYPE:
			return_val.as_int = popi();
			break;
		default:;
	}
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
	for(int i = 0; i < get_child_count(ast); i++) {
		int pos = get_data(get_child(ast, i));
		storei(pos, get_parami(i));
	}
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
	trace("i2r");
    rec_run_ast(get_child(ast, 0));
	pushf((float)popi());
}

void run_r2i(AST* ast) {
	trace("r2i");
    rec_run_ast(get_child(ast, 0));
	pushi((int)popf());
}

void rec_run_ast(AST *ast) {
	if(!ret_abort)
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
