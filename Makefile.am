ACLOCAL_AMFLAGS = -I m4
SUBDIRS = include src

lib_LTLIBRARIES = libSonatinaTag.la
libSonatinaTag_la_SOURCES =
nodist_EXTRA_libSonatinaTag_la_SOURCES = dummy.c
libSonatinaTag_la_LDFLAGS = -version-info 1:0:0

libSonatinaTag_la_LIBADD = src/base/libSTbase.la \
                           src/flac/libSTFLAC.la \
                           src/id3v1/libSTID3v1.la \
                           src/id3v2/libSTID3v2.la \
                           src/m4a/libSTM4A.la \
                           src/utils/libSTutils.la \
                           src/ape/libSTAPE.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = SonatinaTag.pc
