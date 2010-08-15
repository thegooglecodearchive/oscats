## Makefile.am -- Process this file with automake to produce Makefile.in

EXTRA_DIST = README		\
		ex01.py ex01.pl ex01.php ex01.java ex01_java.m

bin_PROGRAMS = ex01
ex01_CFLAGS = -I$(top_srcdir)/src/liboscats @GLIB_CFLAGS@ @GSL_CFLAGS@
ex01_LDADD = $(top_builddir)/src/liboscats/liboscats.la

