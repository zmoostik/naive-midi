AC_PREREQ(2.59)
AC_INIT([naive-midi-player], [0.1.1])
AM_INIT_AUTOMAKE([1.11 foreign -Wall -Wno-portability tar-ustar dist-xz no-dist-gzip])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

#####################################################################
# Program checks
#####################################################################

AC_PROG_CC
AC_PROG_INSTALL

AC_PROG_RANLIB

AC_ARG_ENABLE([sampler],
	[AS_HELP_STRING([--disable-sampler],[Build with Sampler module])],
	[enable_sampler="$enableval"],
	[enable_sampler="yes"])

AM_CONDITIONAL([ENABLE_SAMPLER], [test "x$enable_sampler" != "xno"])

AC_ARG_ENABLE([buzzer],
	[AS_HELP_STRING([--enable-buzzer],[Build with Buzzer module])],
	[enable_buzzer="$enableval"],
	[enable_buzzer="no"])

AM_CONDITIONAL([ENABLE_BUZZER], [test "x$enable_buzzer" != "xno"])

#####################################################################
# Generate files from *.in
#####################################################################

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/buzzer/Makefile
    src/sampler/Makefile
])

#####################################################################

AC_OUTPUT
AC_MSG_RESULT([
    >>> $PACKAGE $VERSION <<<

    prefix:             ${prefix}
    sysconfdir:         ${sysconfdir}
    libdir:             ${libdir}
    rootlibdir:         ${rootlibdir}
    includedir:         ${includedir}
    bindir:             ${bindir}

    sampler support:    ${enable_sampler}
    buzzer support:     ${enable_buzzer}
])
