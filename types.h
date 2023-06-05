
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

Type array_to_primitive(Type type);
Type primitive_to_array(Type type);

Type sum(Type type1, Type type2);
Type mul(Type type1, Type type2);
Type op(Type type1, Type type2);
Type assign(Type type1, Type type2);

#endif // TYPES_H
