# vim:ft=automake

ACLOCAL_AMFLAGS = -I m4

# includes append to these:
SUFFIXES =
.PHONY =
TESTS =
XFAIL_TESTS =
CLEANFILES =
DISTCLEANFILES =
bin_PROGRAMS =
dist_bin_SCRIPTS =
noinst_HEADERS =
lib_LTLIBRARIES =
man_MANS =
noinst_LTLIBRARIES =
noinst_PROGRAMS =
noinst_SCRIPTS=
include_HEADERS =
nobase_include_HEADERS =
check_PROGRAMS =
check_SCRIPTS =
EXTRA_HEADERS =
BUILT_SOURCES=
EXTRA_DIST=
EXTRA_SCRIPTS=
CONFIGURE_DEPENDENCIES=
PREFIX= @prefix@

#includes additional rules from aminclude.am
@INC_AMINCLUDE@
DISTCLEANFILES+= aminclude.am
CLEANFILES+= $(dist_bin_SCRIPTS)

TESTS+= ${check_PROGRAMS} ${check_SCRIPTS}

include yatl/include.am
include tests/include.am
include m4/include.am
include src/include.am
include include/libdrizzle-redux/include.am
include pkg/include.am
include docs/include.am

EXTRA_DIST+= AUTHORS
EXTRA_DIST+= COPYING

# Cleanup individual files in order to preserve uninstall/etc order
maintainer-clean-local:
	cd @abs_top_srcdir@; \
	rm Makefile.in; \
	rm aclocal.m4; \
	rm configure; \
	rm config.in; \
	rm m4/libtool.m4; \
	rm m4/ltoptions.m4; \
	rm m4/ltsugar.m4; \
	rm m4/ltversion.m4; \
	rm m4/lt~obsolete.m4; \
	find . -type f -name '*~' -exec rm -f '{}' \; ; \
	cd @abs_builddir@; \
	rm -rf build-aux; \
	rm config.log; \
	rm config.status; \
	rm -f @PACKAGE@-*.tar.gz; \
	rm -f ./pkg/rpm/@PACKAGE@-*.rpm; \
	rm -f ./pkg/deb/@PACKAGE@-*.deb; \
	rm -rf pkg/deb/install

dist_bin_SCRIPTS+= @GENERIC_CONFIG@
