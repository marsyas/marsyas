PROJECT(ZLIB)

# source files for zlib
SET(ZLIB_SRCS
adler32.c
compress.c
crc32.c
deflate.c
gzio.c
infback.c
inffast.c
inflate.c
inftrees.c
trees.c
uncompr.c
zutil.c
)

if (MARSYAS_STATIC)
   ADD_LIBRARY(zlib ${ZLIB_SRCS})
else (MARSYAS_STATIC)
   ADD_LIBRARY(zlib SHARED ${ZLIB_SRCS})
endif (MARSYAS_STATIC) 

install(TARGETS zlib LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)


