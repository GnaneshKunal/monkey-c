#include "hash.h"

hd_t *hd_create(HD_DT dt, uintptr_t *data) {
  hd_t *hd = malloc(sizeof(hd_t));
  hd->dt = dt;
  switch (dt) {
  case HD_STRING_DT:
    hd->str = (char *)data;
    break;
  case HD_INT_DT:
    hd->num = (int32_t)data;
    break;
  case HD_BOOL_DT:
    hd->b = (bool)*data;
    break;
  default:
    free(hd);
    assert("Unknown data type");
  }
  return hd;
}

void hd_destroy(hd_t **h_p) {
  assert(h_p);
  if (*h_p) {
    hd_t *hd = *h_p;
    if (hd->dt == HD_STRING_DT) {
      free(hd->str);
    }
    free(hd);
    *h_p = NULL;
  }
}

he_t *he_create(const char *key, hd_t *data, he_t *next) {
  assert(key);
  assert(data);

  he_t *he = malloc(sizeof(he_t));
  he->key = strdup(key);
  he->data = data;
  he->next = next;
  return he;
}

void he_destroy(he_t **h_p) {
  assert(h_p);
  if (*h_p) {
    he_t *he = *h_p;
    free(he->key);
    hd_destroy(&he->data);
    if (he->next != NULL) {
      he_destroy(&he->next);
    }
    free(he);
    *h_p = NULL;
  }
}

ht_t *ht_create(size_t n) {
  assert(n > 0);
  ht_t *ht = malloc(sizeof(ht_t));
  assert(ht);
  ht->capacity = n;
  ht->entries = calloc(sizeof(he_t *), n);
  assert(ht->entries);
  ht->length = 0;

  return ht;
}

void ht_destroy(ht_t **ht_ptr) {
  assert(ht_ptr);

  if (*ht_ptr) {
    ht_t *ht = *ht_ptr;
    assert(ht);
    for (int i = 0; i < ht->capacity; i++) {
      if (ht->entries[i] != NULL) {
        he_destroy(&ht->entries[i]);
      }
    }
    free(ht->entries);
    free(ht);
    *ht_ptr = NULL;
  }
}

bool ht_add(ht_t *ht, char *key, hd_t *data) {
  assert(ht);
  assert(key);
  assert(data);

  he_t *he = malloc(sizeof(he_t));
  he->key = strdup(key);
  he->data = data;
  he->next = NULL;

  uint32_t hash = gnu_hash((const uint8_t *)key);

  size_t entry_idx = hash % ht->capacity;

  if (ht->entries[entry_idx] == NULL) {
    ht->entries[entry_idx] = he;
  } else {
    he_t *he_cur = ht->entries[entry_idx];
    while (he_cur->next != NULL) {
      he_cur = he_cur->next;
    }
    he_cur->next = he;
  }

  return true;
}

bool ht_exists(ht_t *ht, char *key) {
  assert(ht);
  assert(key);

  uint32_t hash = gnu_hash((const uint8_t *)key);
  size_t entry_idx = hash % ht->capacity;
  size_t key_len = strlen(key);
  he_t *he_cur = ht->entries[entry_idx];

  while (he_cur != NULL) {
    if (strncmp(he_cur->key, key, key_len) == 0) {
      return true;
    }
    he_cur = he_cur->next;
  }

  return false;
}

hd_t *ht_get(ht_t *ht, char *key) {
  assert(ht);
  assert(key);

  uint32_t hash = gnu_hash((const uint8_t *)key);
  size_t entry_idx = hash % ht->capacity;
  size_t key_len = strlen(key);
  he_t *he_cur = ht->entries[entry_idx];

  while (he_cur != NULL) {
    if (strncmp(he_cur->key, key, key_len) == 0) {
      return he_cur->data;
    }
    he_cur = he_cur->next;
  }

  return NULL;
}

bool ht_delete(ht_t *ht, char *key) {
  assert(ht);
  assert(key);

  uint32_t hash = gnu_hash((const uint8_t *)key);
  size_t entry_idx = hash % ht->capacity;
  size_t key_len = strlen(key);

  he_t *he_cur = ht->entries[entry_idx];

  while (he_cur != NULL) {
    if (strncmp(he_cur->key, key, key_len) == 0) {
      he_destroy(&ht->entries[entry_idx]);
      return true;
    }
    he_cur = he_cur->next;
  }

  return false;
}
