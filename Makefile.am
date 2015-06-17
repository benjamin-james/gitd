AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS = gitd
gitd_SOURCES = gitd.c
gitd_CFLAGS = -pedantic -Wall
gitd_CPPFLAGS = -DCONFDIR="\"$(sysconfdir)"\"

dist_man_MANS = gitd.8
dist_sysconf_DATA = gitd.conf

install-data-local:
	$(MKDIR_P) ${pkgdatadir}
