dnl config.m4 for extension teds

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([teds],
dnl   [for teds support],
dnl   [AS_HELP_STRING([--with-teds],
dnl     [Include teds support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([teds],
  [whether to enable teds support],
  [AS_HELP_STRING([--enable-teds],
    [Enable teds support])])

if test "$PHP_TEDS" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, TEDS_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])
  
  AC_CHECK_HEADERS([stdbool.h],, AC_MSG_ERROR([stdbool.h not exists]))

  AC_MSG_CHECKING(PHP version)

  if test -n "$phpincludedir" -a -d "$phpincludedir"; then
    TEDS_PHPINCLUDEDIR=$phpincludedir
  else
    TEDS_PHPINCLUDEDIR=$abs_srcdir
  fi
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
  #include <$TEDS_PHPINCLUDEDIR/main/php_version.h>
  ]], [[
#if PHP_MAJOR_VERSION < 8
#error PHP < 8
#endif
  ]])],[
  AC_MSG_RESULT([PHP 8])
  ],[
  AC_MSG_ERROR([teds requires php 8.0+])
  ])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-teds -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/teds.h"  # you most likely want to change this
  dnl if test -r $PHP_TEDS/$SEARCH_FOR; then # path given as parameter
  dnl   TEDS_DIR=$PHP_TEDS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for teds files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TEDS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TEDS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the teds distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-teds -> add include path
  dnl PHP_ADD_INCLUDE($TEDS_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-teds -> check for lib and symbol presence
  dnl LIBNAME=TEDS # you may want to change this
  dnl LIBSYMBOL=TEDS # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_TEDS_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your teds library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TEDS_DIR/$PHP_LIBDIR, TEDS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TEDS_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your teds library.])
  dnl ],[
  dnl   -L$TEDS_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(TEDS_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_TEDS, 1, [ Have teds support ])

  PHP_NEW_EXTENSION(teds, teds.c teds_immutablesequence.c teds_immutablekeyvaluesequence.c teds_keyvaluevector.c teds_vector.c teds_deque.c teds_sortedstrictmap.c teds_sortedstrictset.c teds_stableheap.c teds_strictmap.c teds_strictset.c teds_stablesortedlistset.c teds_stablesortedlistmap.c teds_lowmemoryvector.c teds_intvector.c teds_bitset.c teds_interfaces.c, $ext_shared)
fi
