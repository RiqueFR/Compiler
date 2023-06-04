
#include <stdio.h>
#include "types.h"

const Type SUM[7][7] = {
	{INT_TYPE, 	REAL_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR} 	// char**
};
const Type MUL[7][7] = {
	{INT_TYPE, 	REAL_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR} 	// char**
};
const Type OP[7][7] = {
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		INT_TYPE, 	ERROR, 	ERROR, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR} 	// char**
};
const Type ASSIGN_TABLE[7][7] = {
	{INT_TYPE, 	INT_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int
	{REAL_TYPE, REAL_TYPE, 	ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float
	{ERROR, 	ERROR, 		STR_TYPE, 	ERROR, 	ERROR, 	ERROR, 	ERROR}, // char*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // void
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // int*
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR}, // float* 
	{ERROR, 	ERROR, 		ERROR, 		ERROR, 	ERROR, 	ERROR, 	ERROR} 	// char** 
};

static const char *TYPE_STRING[] = {
    "int",
    "float",
    "char*",
    "void",
	"int*",
	"float*",
	"char**",
	"error"
};
static const int TYPE_VALUE[] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7
};

const char* get_text(Type type) {
    return TYPE_STRING[type];
}

const int get_value(Type type) {
	return TYPE_VALUE[type];
}

Type array_to_primitive(Type type) {
	switch(type) {
		case ARRAY_INT_TYPE:
			return INT_TYPE;
			break;
		case ARRAY_REAL_TYPE:
			return  REAL_TYPE;
			break;
		case ARRAY_STR_TYPE:
			return  STR_TYPE;
			break;
		default:
			return  ERROR;
	}
}

Type primitive_to_array(Type type) {
	switch(type) {
		case INT_TYPE:
			return ARRAY_INT_TYPE;
			break;
		case REAL_TYPE:
			return ARRAY_REAL_TYPE;
			break;
		case STR_TYPE:
			return ARRAY_STR_TYPE;
			break;
		default:
			return ERROR;
	}
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
