#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>

/* stack trace */
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

static void handler(int sig) {
  void *array[10];
  size_t size;

  size = backtrace(array, 10);

  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

/* DT_GNU_HASH */
static uint32_t gnu_hash(const uint8_t *name) {
  uint32_t h = 5381;

  for (; *name; name++) {
    h = (h << 5) + h + *name;
  }
  return h;
}

#endif
