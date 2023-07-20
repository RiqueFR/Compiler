
#include <stdio.h>
#include "ast.h"
#include "types.h"

const Unif SUM[6][6] = {
	/* int */ 					/* float */ 				/* char* */ 			/* bool */ 				/* void */ 				/* array */
	{{INT_TYPE, NONE, NONE}, 	{REAL_TYPE, I2F, NONE}, 	{ERROR, NONE, NONE}, 	{INT_TYPE, NONE, B2I}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, 	{REAL_TYPE, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{REAL_TYPE, NONE, B2F}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // char*
	{{INT_TYPE, B2I, NONE}, 	{REAL_TYPE, B2F, NONE}, 	{ERROR, NONE, NONE}, 	{INT_TYPE, B2I, B2I}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // bool
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}  // array
};
const Unif MUL[7][7] = {
	/* int */ 					/* float */ 				/* char* */ 			/* bool */ 				/* void */ 				/* array */
	{{INT_TYPE, NONE, NONE}, 	{REAL_TYPE, I2F, NONE}, 	{ERROR, NONE, NONE}, 	{INT_TYPE, NONE, B2I}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, 	{REAL_TYPE, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{REAL_TYPE, NONE, B2F}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // char*
	{{INT_TYPE, B2I, NONE}, 	{REAL_TYPE, B2F, NONE}, 	{ERROR, NONE, NONE}, 	{INT_TYPE, B2I, B2I}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // bool
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}  // array
};
const Unif OP[7][7] = {
	/* int */ 					/* float */ 				/* char* */ 			/* bool */ 				/* void */ 				/* array */
	{{BOOL_TYPE, NONE, NONE}, 	{BOOL_TYPE, I2F, NONE}, 	{ERROR, NONE, NONE}, 	{BOOL_TYPE, I2B, NONE}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // int
	{{BOOL_TYPE, NONE, I2F}, 	{BOOL_TYPE, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{BOOL_TYPE, F2B, NONE}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // char*
	{{BOOL_TYPE, NONE, I2B}, 	{BOOL_TYPE, NONE, F2B}, 	{ERROR, NONE, NONE}, 	{BOOL_TYPE, NONE, NONE},{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // bool
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}  // array
};
const Unif ANDOR[7][7] = {
	/* int */ 					/* float */ 				/* char* */ 			/* bool */ 				/* void */ 				/* array */
	{{BOOL_TYPE, I2B, I2B}, 	{BOOL_TYPE, I2B, F2B}, 		{ERROR, NONE, NONE}, 	{BOOL_TYPE, I2B, NONE}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // int
	{{BOOL_TYPE, F2B, I2B}, 	{BOOL_TYPE, F2B, F2B}, 		{ERROR, NONE, NONE}, 	{BOOL_TYPE, F2B, NONE}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // char*
	{{BOOL_TYPE, NONE, I2B}, 	{BOOL_TYPE, NONE, F2B}, 	{ERROR, NONE, NONE}, 	{BOOL_TYPE, NONE, NONE},{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // bool
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}  // array
};
const Unif ASSIGN_TABLE[7][7] = {
	/* int */ 					/* float */ 				/* char* */ 			/* bool */ 				/* void */ 				/* array */
	{{INT_TYPE, NONE, NONE}, 	{INT_TYPE, NONE, F2I}, 		{ERROR, NONE, NONE}, 	{INT_TYPE, NONE, B2I}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, 	{REAL_TYPE, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{REAL_TYPE, NONE, B2F}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{STR_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // char*
	{{BOOL_TYPE, NONE, I2B}, 	{BOOL_TYPE, NONE, F2B}, 	{ERROR, NONE, NONE}, 	{BOOL_TYPE, NONE, NONE},{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // bool
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 		{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}, 	{ERROR, NONE, NONE}}  // array
};

static const char *TYPE_STRING[] = {
    "int",
    "float",
    "char*",
    "bool",
    "void",
	"array",
	"error"
};
static const int TYPE_VALUE[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6
};
static const char *TYPE_LLVM[] = {
	"i32",
	"float",
	"i8*",
	"i1",
	"void"
};

const char* get_text(Type type) {
    return TYPE_STRING[type];
}

const char* get_llvm_type(Type type) {
    return TYPE_LLVM[type];
}

const int get_value(Type type) {
	return TYPE_VALUE[type];
}

Unif sum(Type type1, Type type2) {
	return SUM[get_value(type1)][get_value(type2)];
}
Unif mul(Type type1, Type type2) {
	return MUL[get_value(type1)][get_value(type2)];
}
Unif op(Type type1, Type type2) {
	return OP[get_value(type1)][get_value(type2)];
}
Unif logic(Type type1, Type type2) {
	return ANDOR[get_value(type1)][get_value(type2)];
}
Unif assign(Type type1, Type type2) {
	return ASSIGN_TABLE[get_value(type1)][get_value(type2)];
}
