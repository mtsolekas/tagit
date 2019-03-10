AC_INIT([tagit], [master], [https://github.com/mtsolekas/tagit/issues])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/tagit.c])
AC_CONFIG_HEADERS([config.h])
AM_SILENT_RULES([yes])

AC_PROG_CC

# Add --enable-debug flag
AC_ARG_ENABLE([debug],
[ --enable-debug[[=ARG]] Turn on debugging [[ARG=yes]]],
[case "${enableval}" in
 yes) debug=true ;;
 NULL) debug=true ;;
 no) debug=false ;;
 *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac], [debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

# Checks for libraries.
PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES(TAGLIB, taglib_c)
AC_SUBST(TAGLIB_CFLAGS)
AC_SUBST(TAGLIB_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strdup strndup strstr])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
