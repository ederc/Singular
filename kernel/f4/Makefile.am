ACLOCAL_AMFLAGS=-I ../../m4
AM_CPPFLAGS=-I${top_srcdir} -I${top_builddir} -I${top_srcdir}/libpolys -I${top_builddir}/libpolys \
${FACTORY_INCLUDES} $(OMALLOC_INCLUDES) $(RESOURCES_INCLUDES) \
${GMP_CPPFLAGS}

noinst_LTLIBRARIES=libf4.la
libf4_la_SOURCES=gb.c

libf4_la_includedir=$(includedir)/singular/kernel/f4
libf4_la_include_HEADERS=data.h sort_r.h stat.h timer.h



TESTS_ENVIRONMENT = SINGULARPATH='${abs_top_builddir}/libpolys/polys/.libs:${abs_top_builddir}/factory/gftables'
TESTS_ENVIRONMENT += SINGULAR_ROOT_DIR='${abs_top_builddir}'

TESTS = test
check_PROGRAMS = $(TESTS)
test_SOURCES = test.cc
test_LDADD   = libf4.la ${builddir}/../combinatorics/libcombinatorics.la ${builddir}/../linear_algebra/liblinear_algebra.la ${builddir}/../libkernelCommon.la  ${builddir}/../../Singular/libSingular.la

CLEANFILES = $(TESTS)

