#ifndef HASH_H
#define HASH_H

#include "utils.h"

#define SIZE 10000

/* Hash Data */
typedef enum {
  HD_STRING_DT,
  HD_INT_DT,
  HD_BOOL_DT,
} HD_DT;

typedef struct {
  HD_DT dt;
  union {
    char *str;
    int32_t num;
    bool b;
  };
} hd_t;

hd_t *hd_create(HD_DT dt, uintptr_t *data);
void hd_destroy(hd_t **h_p);

/* Hash entry */
typedef struct _he_t {
  char *key;
  hd_t *data;
  struct _he_t *next;
} he_t;

he_t *he_create(const char *key, hd_t *data, he_t *next);
void he_destroy(he_t **h_p);

/* Hash table */
struct _ht_t {
  he_t **entries;
  size_t capacity;
  size_t length;
};

typedef struct _ht_t ht_t;

ht_t *ht_create(size_t n);
void ht_destroy(ht_t **ht_ptr);
bool ht_add(ht_t *ht, char *key, hd_t *data);
bool ht_exists(ht_t *ht, char *key);
hd_t *ht_get(ht_t *ht, char *key);
bool ht_delete(ht_t *ht, char *key);

#endif
