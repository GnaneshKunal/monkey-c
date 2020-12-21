#ifndef OBJECT_H
#define OBJECT_H

#include "utils.h"

typedef struct _obj_t obj_t;

typedef enum {
  INT_OBJ,
  NULL_OBJ,
  BOOL_OBJ,
} OBJ_TYPE;

const char *obj_type_to_str(OBJ_TYPE ot);

typedef struct {
  int32_t value;
} int_obj_t;

int_obj_t *int_obj_new(int32_t value);
void int_obj_destroy(int_obj_t **obj_p);
char *int_obj_to_string(int_obj_t *obj);

typedef struct {
  bool value;
} bool_obj_t;

static bool_obj_t TRUE_IMPL_BOOL_OBJ = {.value=true};
static bool_obj_t FALSE_IMPL_BOOL_OBJ = {.value=false};

bool bool_obj_to_value(bool_obj_t *obj);
char *bool_obj_to_string(bool_obj_t *obj);
obj_t *native_bool_to_boolean_obj(bool input);

typedef struct {} null_obj_t;

struct _obj_t {
  OBJ_TYPE type;
  union {
    int_obj_t *int_obj;
    null_obj_t *null_obj;
    bool_obj_t *bool_obj;
  };
};

static obj_t TRUE_IMPL_OBJ = {.type=BOOL_OBJ, .bool_obj=&TRUE_IMPL_BOOL_OBJ};
static obj_t FALSE_IMPL_OBJ = {.type=BOOL_OBJ, .bool_obj=&FALSE_IMPL_BOOL_OBJ};
static obj_t NULL_IMPL_OBJ = {.type=NULL_OBJ};

obj_t *obj_new(OBJ_TYPE ot, void *value);
void obj_destroy(obj_t **obj_p);
char *obj_to_string(obj_t *obj);

#endif
