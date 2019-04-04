AM_CPPFLAGS = $(TAGLIB_CFLAGS)
AM_CFLAGS = -Wall -Wextra -Wpedantic -std=gnu99 -O3
AM_LDFLAGS = $(TAGLIB_LIBS)

if DEBUG
AM_CFLAGS += -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer \
             -Og -g3
endif

bin_PROGRAMS = tagit
tagit_SOURCES = tagit.c util.c util.h
