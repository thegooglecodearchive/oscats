## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = -DG_LOG_DOMAIN=\"OSCATS\"

if WINDOWS
LDFLAGS += -no-undefined
endif

lib_LTLIBRARIES = liboscats.la
liboscats_la_SOURCES = bitarray.c gsl.c random.c space.c point.c        \
			administrand.c item.c			 	\
			itembank.c examinee.c marshal.c test.c		\
			algorithm.c covariates.c attributes.c		\
			contmodel.c discrmodel.c integrate.c		\
			models/l1p.c					\
			models/l2p.c					\
			models/l3p.c					\
			models/nominal.c				\
			models/dina.c					\
			models/nida.c					\
			algorithms/pick_rand.c				\
			algorithms/chooser.c				\
			algorithms/closest_diff.c			\
			algorithms/max_fisher.c				\
			algorithms/max_kl_cont.c				\
			algorithms/max_kl_discr.c			\
			algorithms/simulate_theta.c			\
			algorithms/simulate_alpha.c			\
			algorithms/exposure_counter.c			\
			algorithms/class_rates.c			\
			algorithms/estimate_theta.c			\
			algorithms/estimate_alpha.c			\
			algorithms/fixed_length.c
liboscats_la_CFLAGS = @GLIB_CFLAGS@ @GSL_CFLAGS@ -Wall -Werror
liboscats_la_LIBADD = @GLIB_LIBS@ @GSL_LIBS@
liboscatsincludedir = $(includedir)/liboscats
liboscatsinclude_HEADERS = oscats.h bitarray.h gsl.h random.h space.h point.h  \
			   administrand.h item.h			\
			   itembank.h examinee.h marshal.h test.h  \
			   algorithm.h algorithms.h models.h covariates.h  \
			   attributes.h contmodel.h discrmodel.h integrate.h
liboscatsmodelsincludedir = $(liboscatsincludedir)/models
liboscatsmodelsinclude_HEADERS = models/l1p.h				\
			models/l2p.h					\
			models/l3p.h					\
			models/nominal.h				\
			models/dina.h					\
			models/nida.h
liboscatsalgincludedir = $(liboscatsincludedir)/algorithms
liboscatsalginclude_HEADERS = algorithms/pick_rand.h                    \
			algorithms/chooser.h				\
			algorithms/closest_diff.h			\
			algorithms/max_fisher.h				\
			algorithms/max_kl_cont.h			\
			algorithms/max_kl_discr.h			\
			algorithms/simulate_theta.h			\
			algorithms/simulate_alpha.h			\
			algorithms/exposure_counter.h			\
			algorithms/class_rates.h			\
			algorithms/estimate_theta.h			\
			algorithms/estimate_alpha.h			\
			algorithms/fixed_length.h
