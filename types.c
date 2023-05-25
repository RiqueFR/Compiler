
#include <stdio.h>
#include "types.h"

const Type SUM[4][4] = {
	{INT_TYPE, REAL_TYPE, ERROR, ERROR}, // int
	{REAL_TYPE, REAL_TYPE, ERROR, ERROR}, // real
	{ERROR, ERROR, ERROR, ERROR}, // string
	{ERROR, ERROR, ERROR, ERROR} //void
};
const Type MUL[4][4] = {
	{INT_TYPE, REAL_TYPE, ERROR, ERROR}, // int
	{REAL_TYPE, REAL_TYPE, ERROR, ERROR}, // real
	{ERROR, ERROR, ERROR, ERROR}, // string
	{ERROR, ERROR, ERROR, ERROR} //void
};
const Type OP[4][4] = {
	{INT_TYPE, INT_TYPE, ERROR, ERROR}, // int
	{INT_TYPE, INT_TYPE, ERROR, ERROR}, // real
	{ERROR, ERROR, INT_TYPE, ERROR}, // string
	{ERROR, ERROR, ERROR, ERROR} //void
};
const Type ASSIGN_TABLE[4][4] = {
	{INT_TYPE, INT_TYPE, ERROR, ERROR}, // int
	{REAL_TYPE, REAL_TYPE, ERROR, ERROR}, // real
	{ERROR, ERROR, STR_TYPE, ERROR}, // string
	{ERROR, ERROR, ERROR, ERROR} //void
};

static const char *TYPE_STRING[] = {
    "int",
    "real",
    "string",
    "void",
	"error"
};
static const int TYPE_VALUE[] = {
	0,
	1,
	2,
	3,
	4
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
