#include "object.h"

const char *obj_type_to_str(OBJ_TYPE ot) {
  switch (ot) {
  case INT_OBJ:
    return "INTEGER";
  case BOOL_OBJ:
    return "BOOLEAN";
  case NULL_OBJ:
    return "NULL";
  case RETURN_VALUE_OBJ:
    return "RETURN";
  case ERROR_OBJ:
    return "ERROR";
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

return_obj_t *return_obj_new(obj_t *value) {
  assert(value);
  return_obj_t *obj = malloc(sizeof(return_obj_t));
  obj->value = value;
  return obj;
}

void return_obj_destroy(return_obj_t **r_obj_p) {
  assert(r_obj_p);
  if (*r_obj_p) {
    return_obj_t *r_obj = *r_obj_p;
    obj_destroy(&r_obj->value);
    free(r_obj);
    *r_obj_p = NULL;
  }
}

char *return_obj_to_string(return_obj_t *r_obj) {
  assert(r_obj);
  return obj_to_string(r_obj->value);
}

error_obj_t *error_obj_new(const char *message) {
  assert(message);
  error_obj_t *error_obj = malloc(sizeof(error_obj_t));
  error_obj->message = strdup(message);
  return error_obj;
}

void error_obj_destroy(error_obj_t **e_obj_p) {
  assert(e_obj_p);
  if (*e_obj_p) {
    error_obj_t *obj = *e_obj_p;
    free(obj->message);
    free(obj);
    *e_obj_p = NULL;
  }
}

char *error_obj_to_string(error_obj_t *e_obj) {
  assert(e_obj);
  char *str = NULL;
  asprintf(&str, "ERROR: %s", e_obj->message);
  return str;
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
  case RETURN_VALUE_OBJ:
    obj = malloc(sizeof(*obj));
    obj->type = ot;
    obj->return_obj = (return_obj_t *)value;
    break;
  case ERROR_OBJ:
    obj = malloc(sizeof(*obj));
    obj->type = ot;
    obj->error_obj = (error_obj_t *)value;
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
    case RETURN_VALUE_OBJ:
      return_obj_destroy(&obj->return_obj);
      free(obj);
      break;
    case ERROR_OBJ:
      error_obj_destroy(&obj->error_obj);
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
  case RETURN_VALUE_OBJ:
    return return_obj_to_string(obj->return_obj);
  case ERROR_OBJ:
    return error_obj_to_string(obj->error_obj);
  }
  return NULL;
}


bool is_truthy(obj_t *obj) {
  switch (obj->type) {
  case BOOL_OBJ:
    return obj->bool_obj->value;
  case NULL_OBJ:
    return false;
  default:
    return true;
  }
}

