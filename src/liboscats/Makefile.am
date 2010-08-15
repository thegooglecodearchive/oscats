## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES = -DG_LOG_DOMAIN=\"OSCATS\"

lib_LTLIBRARIES = liboscats.la
liboscats_la_SOURCES = bitarray.c gsl.c random.c irtmodel.c item.c 	\
			itembank.c examinee.c marshal.c test.c		\
			algorithm.c covariates.c attributes.c		\
			classmodel.c integrate.c			\
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
			algorithms/max_kl_irt.c				\
			algorithms/max_kl_class.c			\
			algorithms/simulate_irt.c			\
			algorithms/simulate_class.c			\
			algorithms/exposure_counter.c			\
			algorithms/class_rates.c			\
			algorithms/estimate_theta.c			\
			algorithms/estimate_class.c			\
			algorithms/fixed_length.c
liboscats_la_CFLAGS = @GLIB_CFLAGS@ @GSL_CFLAGS@ -Wall -Werror
liboscats_la_LIBADD = @GLIB_LIBS@ @GSL_LIBS@
liboscatsincludedir = $(includedir)/liboscats
liboscatsinclude_HEADERS = oscats.h bitarray.h gsl.h random.h irtmodel.h  \
			   item.h itembank.h examinee.h marshal.h test.h  \
			   algorithm.h algorithms.h models.h covariates.h  \
			   attributes.h classmodel.h integrate.h
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
			algorithms/max_kl_irt.h				\
			algorithms/max_kl_class.h			\
			algorithms/simulate_irt.h			\
			algorithms/simulate_class.h			\
			algorithms/exposure_counter.h			\
			algorithms/class_rates.h			\
			algorithms/estimate_theta.h			\
			algorithms/estimate_class.h			\
			algorithms/fixed_length.h
