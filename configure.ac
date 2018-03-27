dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(MultiTerminal, 1.0)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_MKDIR_P
AC_PREFIX_DEFAULT("/usr/local")
# Set exec_prefix default value
#if test "$exec_prefix" = "NONE"; then
#    exec_prefix="$PREFIX/usr"
#fi

AC_ARG_ENABLE(
    debug,
    AS_HELP_STRING(
        [--enable-debug],
        [enable debugging, default: no]
    ),
    [case "${enableval}" in
        yes) debug=true ;;
        no)  debug=false ;;
        *)
            AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_ARG_ENABLE(
    localinstall,
    AS_HELP_STRING(
        [--enable-localinstall],
        [enable local installation, default: no]
    ),
    [case "${enableval}" in
        yes) localinstall=true ;;
        no)  localinstall=false ;;
        *)
            AC_MSG_ERROR([bad value ${enableval} for --enable-localinstall]) ;;
    esac],
[localinstall=false])
if test x"$localinstall" = x"true"; then
    AC_DEFINE(LOCALINSTALL, 1)
fi

AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXFLAGS="$CXXFLAGS -std=c++11"])
ACX_PTHREAD

AM_OPTIONS_WXCONFIG
reqwx=3.1.2
AM_PATH_WXCONFIG($reqwx, wxWin=1,,[core,base,ribbon])
if test "$wxWin" != 1; then
    AC_MSG_ERROR([
        wxWidgets must be installed on your system.
 
        Please check that wx-config is in path, the directory
        where wxWidgets libraries are installed (returned by
        'wx-config --libs' or 'wx-config --static --libs' command)
        is in LD_LIBRARY_PATH or equivalent variable and
        wxWidgets version is $reqwx or above.
        ])
fi

AX_BOOST_BASE([1.58],,
    [AC_MSG_ERROR([MultiTerminal needs Boost, but it was not found])])

PKG_CHECK_MODULES(YAMLCPP, [yaml-cpp >= 0.5])

AC_PROG_CXX

AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

