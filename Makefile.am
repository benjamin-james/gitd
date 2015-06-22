AUTOMAKE_OPTIONS = foreign
CLEANFILES = $(NULL)
edit_script = $(SED) \
-e 's,%datadir%,$(datadir),'g \
$(NULL)

AM_CFLAGS = -W -Wall -Werror -g
bin_PROGRAMS = gitd
gitd_SOURCES = gitd.c
gitd_CPPFLAGS = -DCONFDIR="\"$(sysconfdir)"\"

dist_man_MANS = gitd.8
dist_sysconf_DATA = gitd.conf

gitd.conf: gitd.conf.in Makefile
	$(AM_V_GEN)rm -f $@ $@.tmp && \
	$(edit_script) $< >$@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@
CLEANFILES += gitd.conf
install-data-local:
	$(MKDIR_P) ${DESTDIR}/${pkgdatadir}
