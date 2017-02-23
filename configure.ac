AC_PREREQ([2.69])
AC_INIT([Growbox Automation Project], [1.0],
  [ciborgue@github.com], [cc1101d], [http://github.com/ciborgue/cc1101d])
AC_CONFIG_SRCDIR([src/cc1101d.cc])
AC_CONFIG_HEADERS([include/config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX

AC_CHECK_HEADERS([stdio.h fcntl.h stdint.h syslog.h unistd.h semaphore.h])

AC_CHECK_LIB([pthread], [sem_open])
AC_CHECK_LIB([wiringPi], [wiringPiSetupSys])

AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

AC_CHECK_FUNCS([clock_gettime pow strtol fcntl])

AC_DEFINE([TEXT_BUFFER_LENGTH], [768], [Default snprintf buffer])
AC_DEFINE([DEFAULT_USEC_DELAY], [30000], [Default delay usec])
AC_DEFINE([DEFAULT_RETRY_COUNT], [32], [Default retry count])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
])
AC_OUTPUT
