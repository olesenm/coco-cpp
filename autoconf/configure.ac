#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([Coco/R C++], [20090103], [Loeberbauer AT ssw.jku.at], [coco-cpp])
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_SRCDIR([src/Coco.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h memory.h stddef.h stdlib.h string.h wchar.h])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile:autoconf/Makefile.in src/Makefile])

AC_PROG_INSTALL

AC_OUTPUT

# -----------------------------------------------------------------------------
