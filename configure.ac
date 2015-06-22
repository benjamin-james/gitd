#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([gitd], [0.4.0], [benjamin-james@utulsa.edu])
AC_CONFIG_SRCDIR([gitd.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_SUBST([PACKAGE])
AC_CONFIG_FILES([gitd.conf.in])
AC_SUBST([prefix])
AC_CONFIG_FILES([gitd.service])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_SED

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([strchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
