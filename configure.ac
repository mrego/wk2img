AC_INIT([wk2img], [0.0.1])

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AM_CONFIG_HEADER([config.h])

AC_PROG_CC
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_HEADER_STDC

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])


#*******
# GNOME
#*******

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])
GNOME_MAINTAINER_MODE_DEFINES


#*************
# Dependencies
#*************

PKG_CHECK_MODULES([WK2IMG], [webkit-1.0])

WK2IMG_LIBS="$WK2IMG_LIBS"
WK2IMG_CFLAGS="$WK2IMG_CFLAGS"

AC_SUBST([WK2IMG_LIBS])
AC_SUBST([WK2IMG_CFLAGS])


#*******
# Output
#*******

AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT


#********
# Summary
#********

echo ""
echo "$PACKAGE_STRING"
echo "============="
echo ""
