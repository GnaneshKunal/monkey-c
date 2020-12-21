#include "object.h"

const char *obj_type_to_str(OBJ_TYPE ot) {
  switch (ot) {
  case INT_OBJ:
    return "INTEGER";
  case BOOL_OBJ:
    return "BOOLEAN";
  case NULL_OBJ:
    return "NULL";
  }
}

int_obj_t *int_obj_new(int32_t value) {
  int_obj_t *obj = malloc(sizeof(*obj));
  assert(obj);
  obj->value = value;
  return obj;
}

void int_obj_destroy(int_obj_t **obj_p) {
  assert(obj_p);
  if (*obj_p) {
    int_obj_t *obj = *obj_p;
    free(obj);
    *obj_p = NULL;
  }
}

char *int_obj_to_string(int_obj_t *obj) {
  assert(obj);
  char *str = NULL;
  asprintf(&str, "%" PRId32, obj->value);
  return str;
}

bool bool_obj_to_value(bool_obj_t *obj) {
  return obj->value;
}

obj_t *native_bool_to_boolean_obj(bool input) {
  return input ? &TRUE_IMPL_OBJ : &FALSE_IMPL_OBJ;
}

char *bool_obj_to_string(bool_obj_t *obj) {
  assert(obj);
  return strdup(get_bool_literal(obj->value));
}

obj_t *obj_new(OBJ_TYPE ot, void *value) {
  obj_t *obj = NULL;
  switch (ot) {
  case INT_OBJ:
    obj = malloc(sizeof(*obj));
    obj->type = ot;
    obj->int_obj = (int_obj_t *)value;
    break;
  /* case NULL_OBJ: */
  /*   obj->null_obj = (null_obj_t *)value; */
  /*   break; */
  /* case BOOL_OBJ: */
  /*   obj->bool_obj = (bool_obj_t *)value; */
    /* break; */
  default:
    assert("Unknown object");
  }
  return obj;
}

void obj_destroy(obj_t **obj_p) {
  assert(obj_p);
  if (*obj_p) {
    obj_t *obj = *obj_p;
    switch (obj->type) {
    case INT_OBJ:
      int_obj_destroy(&obj->int_obj);
      free(obj);
      break;
    case NULL_OBJ:
      /*
       * Do nothing. Null obj is not dynamically allocated.
       */
      break;
    case BOOL_OBJ:
      /*
       * Do nothing. Boolean obj is not dynamically allocated.
       */
      break;
    }
    *obj_p = NULL;
  }
}

char *obj_to_string(obj_t *obj) {
  assert(obj);
  switch (obj->type) {
  case INT_OBJ:
    return int_obj_to_string(obj->int_obj);
  case NULL_OBJ:
    return strdup("null");
  case BOOL_OBJ:
    return bool_obj_to_string(obj->bool_obj);
  }
  return NULL;
}
