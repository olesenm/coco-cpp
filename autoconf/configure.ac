#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([Coco/R C++], [20100310], [Loeberbauer AT ssw.jku.at], [coco-cpp])
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_SRCDIR([src/Coco.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX

AC_PROG_INSTALL

# Checks for header files.

#### # this section is currently disabled since we don't yet use config.h
#### AC_CHECK_HEADERS([fcntl.h memory.h stddef.h stdlib.h string.h wchar.h])
####
#### # Checks for library functions.
#### AC_FUNC_MALLOC
#### AC_FUNC_STRTOD
#### AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile:autoconf/Makefile.in src/Makefile])

AC_OUTPUT

# -----------------------------------------------------------------------------
