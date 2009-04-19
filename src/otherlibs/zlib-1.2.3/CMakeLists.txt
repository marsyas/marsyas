PROJECT(ZLIB)

# source files for zlib
SET(ZLIB_SRCS
adler32.c
compress.c
crc32.c
deflate.c
example.c
gzio.c
infback.c
inffast.c
inflate.c
inftrees.c
minigzip.c
trees.c
uncompr.c
zutil.c
)
ADD_LIBRARY(zlib ${ZLIB_SRCS})


