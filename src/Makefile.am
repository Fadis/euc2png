bin_PROGRAMS = euc2png
euc2png_SOURCES = draw.c euc2png.c glyph.c map.c

# set the include path found by configure
INCLUDES= $(all_includes)

euc2png_CXXFLAGS= @IL_CFLAGS@

# the library search path.
euc2png_LDFLAGS = $(all_libraries) @IL_LIBS@
noinst_HEADERS = draw.h glyph.h map.h
