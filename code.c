
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

typedef union {
    int   as_int;
    float as_float;
} Word;

// ----------------------------------------------------------------------------
// Code memory ----------------------------------------------------------------

int next_instr;

// ----------------------------------------------------------------------------

#define MAX_STR_SIZE 128
static char str_buf[MAX_STR_SIZE];
#define clear_str_buf() str_buf[0] = '\0'

// Helper function to write strings.
int escape_str(const char* s, char *n) { 
    int i = 0, j = 0;
    char c;
	int size = 0;
    while ((c = s[i++]) != '\0') { 
        if (c == '"') {
			continue;
		} else if (c == '\n') {
            n[j++] = '\\';
            n[j++] = '0';
            n[j++] = 'A';
		} else if (c == '\\' && s[i] == 'n') { 
            n[j++] = '\\';
            n[j++] = '0';
            n[j++] = 'A';
            i++;
        } else { 
            n[j++] = c;
        } 
		size++;
    } 
    n[j] = '\0';
	return size;
}

void emit(const char* instr) {
	printf("%s\n", instr);
	next_instr++;
}

void store_reg(Type type, int dest_reg, int value_reg) {
	char str[500];
	int align = 4;
	if (type == STR_TYPE) align = 8;
	sprintf(str, "store %s %%%d, %s* %%%d, align %d", get_llvm_type(type), value_reg, get_llvm_type(type), dest_reg, align);
	emit(str);
}

// ----------------------------------------------------------------------------
// Prints ---------------------------------------------------------------------

#define LINE_SIZE 80
#define MAX_STR_SIZE 128


void dump_program() {
    /*for (int addr = 0; addr < next_instr; addr++) {*/
        /*write_instruction(addr);*/
    /*}*/
}

// TODO fix char encodes like \n
void dump_str_table() {
    int table_size = get_str_table_size(str_table);
	for (int i = 0; i < table_size; i++) {
		char* str = get_string(str_table, i);
		clear_str_buf();
		int size = escape_str(str, str_buf);
        printf("@.str.%d = private unnamed_addr constant [%d x i8] c\"%s\\00\", align 1\n", i, 1+size, str_buf);
    }
}

// ----------------------------------------------------------------------------
// AST Traversal --------------------------------------------------------------

/*#define TRACE*/
#ifdef TRACE
#define trace(msg) printf("TRACE: %s\n", msg)
#else
#define trace(msg)
#endif

int regs_count;
int jump_label;

int new_reg(const char* str) {
	emit(str);
	return regs_count++;
}

int new_reg_emit(const char* str_emit) {
	char str[500];
	sprintf(str, "%%%d = %s", regs_count, str_emit);
	emit(str);
	return regs_count++;
}

int new_int_reg() {
	char str[500];
	sprintf(str, "%%%d = alloca i32, align 4", regs_count);
	return new_reg(str);
}
int new_float_reg() {
	char str[500];
	sprintf(str, "%%%d = alloca float, align 4", regs_count);
	return new_reg(str);
}
int new_str_reg() {
	char str[500];
	sprintf(str, "%%%d = alloca i8*, align 8", regs_count);
	return new_reg(str);
}

int emit_load(int reg, Type type) {
	char str[500];
	const char* type_llvm = get_llvm_type(type);
	int align = 4;
	if(type == STR_TYPE) align = 8;
	sprintf(str, "load %s, %s* %%%d, align %d", type_llvm, type_llvm, reg, align);
	return new_reg_emit(str);
}

int rec_emit_code(AST *ast);

// ----------------------------------------------------------------------------

// TODO find a way to do and
int emit_and(AST* ast) {
	trace("and");
	int x = rec_emit_code(get_child(ast, 0));
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	sprintf(str, "icmp ne i32 %%%d, %%%d", x, y);
	int reg = new_reg_emit(str);
	sprintf(str, "zext i1 %%%d to i32", reg);
	return new_reg_emit(str);
}

int emit_array_use(AST* ast) {
	trace("array_use");
	return -1;
}

int emit_assign(AST *ast) {
	trace("assign");
	AST* expr_node = get_child(ast, 1);
	int reg_expr = rec_emit_code(expr_node);
	int idx = get_data(get_child(ast, 0));
	store_reg(get_node_type(expr_node), 1+get_var_offset(var_table, idx), reg_expr);
	return -1;
}

int emit_block(AST *ast) {
	trace("block");
	int size = get_child_count(ast);
	for(int i = 0; i < size; i++) {
		AST* child = get_child(ast, i);
		rec_emit_code(child);
		if(get_kind(child) == RETURN_NODE && i != size - 1) {
			char str[500];
			sprintf(str, "%d:", regs_count++);
			emit(str);
		}
	}
	return -1;
}

int emit_eq(AST *ast) {
	trace("eq");
	int x = rec_emit_code(get_child(ast, 0));
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	sprintf(str, "icmp eq i32 %%%d, %%%d", x, y);
	int reg = new_reg_emit(str);
	sprintf(str, "zext i1 %%%d to i32", reg);
	return new_reg_emit(str);
}

int emit_func_decl(AST* ast) {
	trace("func_decl");
	int func_idx = get_data(ast);
	char str[600];
	char args[500];
	args[0] = '\0';
	AST* var_list = get_child(ast, 0);
	int num_vars = get_child_count(var_list);
	emit("; Function Attrs: noinline nounwind optnone uwtable");
	for(int i = 0; i < num_vars; i++) {
		Type var_type = get_node_type(get_child(var_list, i));
		char arg[500];
		sprintf(arg, "%s noundef %%param%d", get_llvm_type(var_type), i);
		strcat(args, arg);
		if (i != num_vars - 1) {
			strcat(args, ", ");
		}
	}
	sprintf(str, "define dso_local %s @%s(%s) #0 {", get_llvm_type(get_node_type(ast)), get_func_name(func_table, func_idx), args);
	emit(str);
	// allocate all vals
	regs_count = 1;//num_vars+1;
	for(int i = 0; i < get_var_table_size(var_table); i++) {
		if(get_scope(var_table, i) == func_idx) {
			switch (get_type(var_table, i)) {
				case INT_TYPE:
					new_int_reg();
					break;
				case REAL_TYPE:
					new_float_reg();
					break;
				case STR_TYPE:
					new_str_reg();
					break;
				default:break;
			}
		}
	}
	for(int i = 0; i < num_vars; i++) {
		Type var_type = get_node_type(get_child(var_list, i));
		int var_idx = get_data(get_child(var_list, i));
		int align = 4;
		if(var_type == STR_TYPE) align = 8;
		char str[500];
		sprintf(str, "store %s %%param%d, %s* %%%d, align %d", get_llvm_type(var_type), i, get_llvm_type(var_type), 1+get_var_offset(var_table, var_idx), align);
		emit(str);
	}
	rec_emit_code(get_child(ast, 1));
	if(get_node_type(ast) == VOID_TYPE) emit("ret void");
	emit("}");
	return -1;
}

// TODO \n do not work
int emit_print(AST* ast) {
	trace("print");
	AST* child = get_child(ast, 0);
	Type child_type = get_node_type(child);
	int child_reg = rec_emit_code(child);
	char emit_str[500];
	int print_str_idx = add_string(str_table, "%s");
	switch (child_type) {
		case STR_TYPE:
			print_str_idx = add_string(str_table, "%s");
			sprintf(emit_str, "call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str.%d, i64 0, i64 0), %s noundef %%%d)", print_str_idx, get_llvm_type(child_type), child_reg);
			return new_reg_emit(emit_str);
		case INT_TYPE:
			print_str_idx = add_string(str_table, "%d\n");
			break;
		case REAL_TYPE:
			print_str_idx = add_string(str_table, "%f\n");
			sprintf(emit_str, "fpext float %%%d to double", child_reg);
			int float_conv = new_reg_emit(emit_str);
			sprintf(emit_str, "call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.%d, i64 0, i64 0), double noundef %%%d)", print_str_idx, float_conv);
			return new_reg_emit(emit_str);
		default:break;
	}
	sprintf(emit_str, "call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.%d, i64 0, i64 0), %s noundef %%%d)", print_str_idx, get_llvm_type(child_type), child_reg);
	return new_reg_emit(emit_str);
}

// TODO scanf
int emit_func_use(AST* ast) {
	trace("func_use");
	int func_idx = get_data(ast);
	if(!strcmp(get_func_name(func_table, func_idx), "printf")) {
		emit_print(ast);
		return -1;
	}
	char args[500];
	args[0] = '\0';
	int var_num = get_child_count(ast);
	for(int i = 0; i < var_num; i++) {
		AST* child = get_child(ast, i);
		int reg = rec_emit_code(child);
		char str[500];
		sprintf(str, "%s noundef %%%d", get_llvm_type(get_node_type(child)), reg);
		strcat(args, str);
		if(i != var_num-1) strcat(args, ", ");
	}
	Type func_type = get_node_type(ast);
	char emit_str[600];
	if(func_type != VOID_TYPE) {
		sprintf(emit_str, "call %s @%s(%s)", get_llvm_type(func_type), get_func_name(func_table, func_idx), args);
		return new_reg_emit(emit_str);
	}
	sprintf(emit_str, "call void @%s(%s)", get_func_name(func_table, func_idx), args);
	emit(emit_str);
	return -1;
}

int last_node_if_is_return(AST* ast_block) {
	int size = get_child_count(ast_block);
	if(size > 0) {
		AST* last_child = get_child(ast_block, size-1);
		if(get_kind(last_child) == RETURN_NODE) {
			return 1;
		}
	}
	return 0;
}

int emit_if(AST *ast) {
	trace("if");
	int x = rec_emit_code(get_child(ast, 0));
	char str[500];
	sprintf(str, "icmp ne i32 %%%d, 0", x);
	x = new_reg_emit(str);

	int br_true = jump_label++;
	if(get_child_count(ast) > 2) { // has else statment
		int br_false = jump_label++;
		int br_end = jump_label++;
		sprintf(str, "br i1 %%%d, label %%jump%d, label %%jump%d", x, br_true, br_false);
		emit(str);

		AST* true_block = get_child(ast, 1);
		sprintf(str, "jump%d:", br_true);
		emit(str);
		rec_emit_code(true_block);
		if(!last_node_if_is_return(true_block)) {
			sprintf(str, "br label %%jump%d", br_end);
			emit(str);
		}

		AST* false_block = get_child(ast, 2);
		sprintf(str, "jump%d:", br_false);
		emit(str);
		rec_emit_code(false_block);
		if(!last_node_if_is_return(false_block)) {
			sprintf(str, "br label %%jump%d", br_end);
			emit(str);
		}

		sprintf(str, "jump%d:", br_end);
		emit(str);
	} else {
		int br_end = jump_label++;
		sprintf(str, "br i1 %%%d, label %%jump%d, label %%jump%d", x, br_true, br_end);
		emit(str);

		AST* true_block = get_child(ast, 1);
		sprintf(str, "jump%d:", br_true);
		emit(str);
		rec_emit_code(true_block);
		if(!last_node_if_is_return(true_block)) {
			sprintf(str, "br label %%jump%d", br_end);
			emit(str);
		}

		sprintf(str, "jump%d:", br_end);
		emit(str);
	}
	return -1;
}

int emit_int_val(AST *ast) {
	trace("int_val");
	int x = new_int_reg();
	int idx = get_data(ast);
	char str[500];
	sprintf(str, "store i32 %d, i32* %%%d, align 4", idx, x);
	emit(str);
	return emit_load(x, INT_TYPE);
}

int emit_gt(AST *ast) {
	trace("gt");
	int x = rec_emit_code(get_child(ast, 0));
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	sprintf(str, "icmp sgt i32 %%%d, %%%d", x, y);
	int reg = new_reg_emit(str);
	sprintf(str, "zext i1 %%%d to i32", reg);
	return new_reg_emit(str);
}

int emit_lt(AST *ast) {
	trace("lt");
	int x = rec_emit_code(get_child(ast, 0));
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	sprintf(str, "icmp slt i32 %%%d, %%%d", x, y);
	int reg = new_reg_emit(str);
	sprintf(str, "zext i1 %%%d to i32", reg);
	return new_reg_emit(str);
}

int emit_minus(AST *ast) {
	trace("minus");
	AST* child = get_child(ast, 0);
	int x = rec_emit_code(child);
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	switch (get_node_type(child)) {
		case REAL_TYPE:
			sprintf(str, "fsub float %%%d, %%%d", x, y);
			break;
		case INT_TYPE:
			sprintf(str, "sub nsw i32 %%%d, %%%d", x, y);
			break;
		default: break;
	}
	int reg = new_reg_emit(str);
	return reg;
}

int emit_neg(AST* ast) {
	trace("neg");
	int x = rec_emit_code(get_child(ast, 0));
	char str[500];
	x = emit_load(x, get_node_type(ast));
	sprintf(str, "sub nsw i32 0, %%%d", x);
	return new_reg_emit(str);
}

// TODO
int emit_not(AST* ast) {
	trace("not");
	return -1;
}

// TODO find a way to do or
int emit_or(AST* ast) {
	trace("or");
	return -1;
}

int emit_over(AST *ast) {
	trace("over");
	AST* child = get_child(ast, 0);
	int x = rec_emit_code(child);
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	switch (get_node_type(child)) {
		case REAL_TYPE:
			sprintf(str, "fdiv float %%%d, %%%d", x, y);
			break;
		case INT_TYPE:
			sprintf(str, "sdiv i32 %%%d, %%%d", x, y);
			break;
		default: break;
	}
	int reg = new_reg_emit(str);
	return reg;
}

int emit_plus(AST *ast) {
	trace("plus");
	AST* child = get_child(ast, 0);
	int x = rec_emit_code(child);
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	switch (get_node_type(child)) {
		case REAL_TYPE:
			sprintf(str, "fadd float %%%d, %%%d", x, y);
			break;
		case INT_TYPE:
			sprintf(str, "add nsw i32 %%%d, %%%d", x, y);
			break;
		default: break;
	}
	int reg = new_reg_emit(str);
	return reg;
}

int emit_program(AST *ast) {
    trace("program");
	rec_emit_code(get_child(ast, 0));
	emit("declare i32 @printf(i8* noundef, ...) #1");
	emit("!6 = distinct !{!6, !7}");
	emit("!7 = !{!\"llvm.loop.mustprogress\"}");
	return -1;
}

int emit_real_val(AST *ast) {
	trace("real_val");
	int x = new_float_reg();
	double value = get_float_data(ast); // llvm seems to use double
	char str[500];
	sprintf(str, "store float 0x%lx, float* %%%d, align 4", *(unsigned long int*)&value, x);
	emit(str);
	return emit_load(x, REAL_TYPE);
}

int emit_return(AST* ast) {
	trace("return");
	if(get_child_count(ast) == 0) { // return void
		emit("ret void");
		return -1;
	}
	char str[500];
	AST* child = get_child(ast, 0);
	Type node_type = get_node_type(child);
	int reg = rec_emit_code(child);
	sprintf(str, "ret %s %%%d", get_llvm_type(node_type), reg);
	emit(str);
	return -1;
}

int emit_while(AST *ast) {
	trace("while");
	int br_expr = jump_label++;
	int br_block = jump_label++;
	int br_end = jump_label++;
	char str[500];
	sprintf(str, "br label %%jump%d", br_expr);
	emit(str);

	sprintf(str, "jump%d:", br_expr);
	emit(str);
	int res = rec_emit_code(get_child(ast, 0));
	sprintf(str, "icmp ne i32 %%%d, 0", res);
	res = new_reg_emit(str);
	sprintf(str, "br i1 %%%d, label %%jump%d, label %%jump%d", res, br_block, br_end);
	emit(str);

	sprintf(str, "jump%d:", br_block);
	emit(str);
	res = rec_emit_code(get_child(ast, 1));
	sprintf(str, "br label %%jump%d, !llvm.loop !6", br_expr);
	emit(str);

	sprintf(str, "jump%d:", br_end);
	emit(str);
	return -1;
}

int emit_str_val(AST *ast) {
    trace("str_val");
	int x = new_str_reg();
	int str_idx = get_data(ast);
	int str_len = escape_str(get_string(str_table, str_idx), str_buf);
	char str[500];
	sprintf(str, "store i8* getelementptr inbounds ([%d x i8], [%d x i8]* @.str.%d, i64 0, i64 0), i8** %%%d, align 8", str_len+1, str_len+1, str_idx, x);
	emit(str);
	return emit_load(x, STR_TYPE);
}

int emit_times(AST *ast) {
	trace("times");
	AST* child = get_child(ast, 0);
	int x = rec_emit_code(child);
	int y = rec_emit_code(get_child(ast, 1));
	char str[500];
	switch (get_node_type(child)) {
		case REAL_TYPE:
			sprintf(str, "fmul float %%%d, %%%d", x, y);
			break;
		case INT_TYPE:
			sprintf(str, "mul i32 %%%d, %%%d", x, y);
			break;
		default: break;
	}
	int reg = new_reg_emit(str);
	return reg;
}

int emit_var_decl(AST *ast) {
    trace("var_decl");
	return -1;
    // Nothing to do, memory was already cleared upon initialization.
}

int emit_var_list(AST *ast) {
    trace("var_list");
    // Nothing to do, memory was already cleared upon initialization.
	return -1;
}

int emit_var_use(AST *ast) {
	trace("var_use");
	return emit_load(1+get_var_offset(var_table, get_data(ast)), get_node_type(ast));
}

int emit_void_val(AST* ast) {
	trace("void_val");
	// Do nothing
	return -1;
}

int emit_i2r(AST* ast) {
	trace("i2r");
	int reg = rec_emit_code(get_child(ast, 0));
	char str[500];
	sprintf(str, "sitofp i32 %%%d to float", reg);
	return new_reg_emit(str);
}

int emit_r2i(AST* ast) {
	trace("r2i");
	int reg = rec_emit_code(get_child(ast, 0));
	char str[500];
	sprintf(str, "fptosi float %%%d to i32", reg);
	return new_reg_emit(str);
}

int rec_emit_code(AST *ast) {
	if(!ret_abort)
		switch(get_kind(ast)) {
			case AND_NODE:   		return emit_and(ast);
			case ARRAY_USE_NODE:   	return emit_array_use(ast);
			case ASSIGN_NODE:   	return emit_assign(ast);
			case BLOCK_NODE:    	return emit_block(ast);
			case EQ_NODE:       	return emit_eq(ast);
			case FLOAT_VAL_NODE: 	return emit_real_val(ast);
			case FUNC_DECL_NODE: 	return emit_func_decl(ast);
			case FUNC_USE_NODE: 	return emit_func_use(ast);
			case IF_NODE: 			return emit_if(ast);
			case INT_VAL_NODE: 		return emit_int_val(ast);
			case GT_NODE: 			return emit_gt(ast);
			case LT_NODE: 			return emit_lt(ast);
			case MINUS_NODE: 		return emit_minus(ast);
			case NEG_NODE: 			return emit_neg(ast);
			case NOT_NODE: 			return emit_not(ast);
			case OR_NODE: 			return emit_or(ast);
			case OVER_NODE: 		return emit_over(ast);
			case PLUS_NODE: 		return emit_plus(ast);
			case PROGRAM_NODE: 		return emit_program(ast);
			case RETURN_NODE: 		return emit_return(ast);
			case STRING_VAL_NODE: 	return emit_str_val(ast);
			case TIMES_NODE: 		return emit_times(ast);
			case VAR_DECL_NODE: 	return emit_var_decl(ast);
			case VAR_LIST_NODE: 	return emit_var_list(ast);
			case VAR_USE_NODE: 		return emit_var_use(ast);
			case VOID_VAL_NODE: 	return emit_void_val(ast);
			case WHILE_NODE: 		return emit_while(ast);

			case I2R_NODE: 			return emit_i2r(ast);
			case R2I_NODE: 			return emit_r2i(ast);

			default:
				fprintf(stderr, "Invalid kind: %s!\n", kind2str(get_kind(ast)));
				exit(EXIT_FAILURE);
		}
	return -1;
}

// ----------------------------------------------------------------------------

void emit_code(AST *ast) {
	next_instr = 0;
    regs_count = 1; // regs start at 1
	jump_label = 1;
    dump_str_table();
    rec_emit_code(ast);
    /*emit0(HALT);*/
    dump_program();
}
