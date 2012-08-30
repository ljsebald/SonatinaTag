#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([SonatinaTag], [1.0.0])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

# Checks for programs.
AC_LANG([C])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset strchr strdup])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/SonatinaTag/Makefile
                 include/SonatinaTag/Tags/Makefile
                 src/Makefile
                 src/base/Makefile
                 src/flac/Makefile
                 src/id3v1/Makefile
                 src/id3v2/Makefile
                 src/m4a/Makefile
                 src/utils/Makefile
                 SonatinaTag.pc])

AC_OUTPUT
