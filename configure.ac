#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Monkey], [0.1], [gnaneshkunal@outlook.com])
AM_PROG_AR

AC_CONFIG_SRCDIR([src/main.c])

AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.11.2])

AC_CONFIG_HEADERS([config.h])

# Checks for programs.
CFLAGS='-g3 -O0 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls'
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lbsd':
AC_CHECK_LIB([bsd], [main])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])
AM_PROG_CC_C_O

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([inttypes.h limits.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostname strdup])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
