pkg_incdir = $(top_srcdir)/include
pkg_libdir = $(top_srcdir)/lib

AM_CFLAGS = -I$(pkg_incdir)

bin_PROGRAMS = git-stashd
include_HEADERS = $(pkg_incdir)/argv.h   \
                  $(pkg_incdir)/common.h \
                  $(pkg_incdir)/daemon.h \
                  $(pkg_incdir)/usage.h

git_stashd_DEPENDENCIES = libgit2
git_stashd_LDADD = $(pkg_libdir)/libgit2/libgit2/build/libgit2.so
git_stashd_SHORTNAME = stashd
git_stashd_SOURCES = src/argv.c   \
                     src/daemon.c \
                     src/usage.c  \
                     src/main.c
