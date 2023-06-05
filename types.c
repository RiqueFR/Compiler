
#include <stdio.h>
#include "types.h"

const Type SUM[5][5] = {
	{INT_TYPE, 	REAL_TYPE, 	ERROR, 		ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}  // array
};
const Type MUL[7][7] = {
	{INT_TYPE, 	REAL_TYPE, 	ERROR, 		ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}  // array
};
const Type OP[7][7] = {
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR}, // int
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		INT_TYPE, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}  // array
};
const Type ASSIGN_TABLE[7][7] = {
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		STR_TYPE, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR}  // array
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

const char* get_text(Type type) {
    return TYPE_STRING[type];
}

const int get_value(Type type) {
	return TYPE_VALUE[type];
}

Type sum(Type type1, Type type2) {
	return SUM[get_value(type1)][get_value(type2)];
}
Type mul(Type type1, Type type2) {
	return MUL[get_value(type1)][get_value(type2)];
}
Type op(Type type1, Type type2) {
	return OP[get_value(type1)][get_value(type2)];
}
Type assign(Type type1, Type type2) {
	return ASSIGN_TABLE[get_value(type1)][get_value(type2)];
}
