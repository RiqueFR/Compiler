
#ifndef TYPES_H
#define TYPES_H

typedef enum {
  INT_TYPE,
  REAL_TYPE,
  STR_TYPE,
  VOID_TYPE,
  ARRAY,
  ERROR
} Type;

const char *get_text(Type type);
const int get_value(Type type);

Typedef enum {
	I2F,
	F2I,
	NONE
} Conv;

Type array_to_primitive(Type type);
Type primitive_to_array(Type type);

typedef struct {
	Type type;
	Conv lc;
	Conv rc;
} Unif;

Unif sum(Type type1, Type type2);
Unif mul(Type type1, Type type2);
Unif op(Type type1, Type type2);
Unif assign(Type type1, Type type2);

#endif // TYPES_H
