#ifndef OBJECT_H
#define OBJECT_H

#include "utils.h"

typedef enum {
  INT_OBJ,
} OBJ_TYPE;

const char *obj_type_to_str(OBJ_TYPE ot);

typedef struct {
  int32_t value;
} int_obj;

int_obj *int_obj_new(int32_t value);
void int_obj_destroy(int_obj **obj_p);
char *int_obj_to_string(int_obj *obj);

typedef struct _obj_t {
  OBJ_TYPE type;
  union {
    int_obj *int_value;
  };
} obj_t;

obj_t *obj_new(OBJ_TYPE ot, void *value);
void obj_destroy(obj_t **obj_p);
char *obj_to_string(obj_t *obj);

#endif
