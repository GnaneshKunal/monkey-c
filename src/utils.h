#ifndef UTILS_H
#define UTILS_H

#ifdef __linux__
#define _GNU_SOURCE

#ifdef HAVE_LIBBSD
#include <bsd/stdio.h>
#include <bsd/stdlib.h>
#include <bsd/string.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <stdint.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include <inttypes.h>           /* contains macros for printing long
                                   integers */

#ifdef HAVE_EXECINFO_H
/* stack trace */
#include <execinfo.h>
#endif
#include <signal.h>
#include <unistd.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif

#define DUMP(varname) fprintf(stdout, "%s = \"%s\"\n", #varname, varname);

#ifdef HAVE_EXECINFO_H
static void handler(int sig) {
  void *array[10];
  size_t size;

  size = backtrace(array, 10);

  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
#endif

/* DT_GNU_HASH */
static uint32_t gnu_hash(const uint8_t *name) {
  uint32_t h = 5381;

  for (; *name; name++) {
    h = (h << 5) + h + *name;
  }
  return h;
}

static void assert_fail(bool value, char **err_msg_p) {
  assert(err_msg_p);
  char *err_msg = *err_msg_p;
  if (!value) {
    puts(err_msg);
  }
  free(err_msg);
  *err_msg_p = NULL;
  assert(value);
}

static const char *get_bool_literal(bool value) {
  return value ? "true" : "false";
}

#endif
