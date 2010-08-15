## Makefile.am -- Process this file with automake to produce Makefile.in

EXTRA_DIST = codegen.pl oscats.pm oscats.override

CLEAN_FILES = oscats.defs oscats.maps lib

oscats.maps: $(srcdir)/../oscats-common.defs $(srcdir)/oscats.override
	if test ! -d lib; then mkdir lib; fi
	if test ! -f lib/oscats.pm; then cp $(srcdir)/oscats.pm lib; fi
	cp $(srcdir)/../oscats-common.defs oscats.defs
	$(PERL) $(srcdir)/codegen.pl $(srcdir)/oscats.override oscats.defs

Build: oscats.maps Build.PL
	top_builddir=$(top_builddir) srcdir=$(srcdir) \
	top_srcdir=$(top_srcdir) $(PERL) Build.PL

blib/arch/auto/oscats/oscats.bundle: Build lib/oscats.xs
	./Build

install-exec-local: blib/arch/auto/oscats/oscats.bundle
	./Build install

clean-local:
	./Build clean

noinst_SCRIPTS = blib/arch/auto/oscats/oscats.bundle
