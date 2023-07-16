
#include <stdio.h>
#include "types.h"

const Unif SUM[5][5] = {
	{{INT_TYPE, NONE, NONE}, {REAL_TYPE, I2F, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, {REAL_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // char*
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}  // array
};
const Unif MUL[7][7] = {
	{{INT_TYPE, NONE, NONE}, {REAL_TYPE, I2F, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, {REAL_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // char*
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}  // array
};
const Unif OP[7][7] = {
	{{INT_TYPE, NONE, NONE}, {INT_TYPE, I2F, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // int
	{{INT_TYPE, NONE, I2F}, {INT_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {INT_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // char*
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}  // array
};
const Unif ASSIGN_TABLE[7][7] = {
	{{INT_TYPE, NONE, NONE}, {INT_TYPE, NONE, F2I}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // int
	{{REAL_TYPE, NONE, I2F}, {REAL_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // float
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {STR_TYPE, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // char*
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}, // void
	{{ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}, {ERROR, NONE, NONE}}  // array
};

static const char *TYPE_STRING[] = {
    "int",
    "float",
    "char*",
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
	5
};
static const char *TYPE_LLVM[] = {
	"i32",
	"float",
	"char*",
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
Unif assign(Type type1, Type type2) {
	return ASSIGN_TABLE[get_value(type1)][get_value(type2)];
}
