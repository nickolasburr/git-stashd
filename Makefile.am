AUTOMAKE_OPTIONS = foreign
pkg_includedir = $(top_srcdir)/include
AM_CFLAGS = -I$(pkg_includedir) $(PKG_CFLAGS)
bin_PROGRAMS = git-stashd
include_HEADERS = $(pkg_includedir)/argv.h $(pkg_includedir)/common.h $(pkg_includedir)/daemon.h $(pkg_includedir)/usage.h
git_stashd_SOURCES = src/argv.c src/daemon.c src/usage.c src/main.c
