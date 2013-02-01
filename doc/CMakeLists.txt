## global setup
cmake_minimum_required(VERSION 2.6)
project(Document)

## program setup
find_program(TEXI2PDF NAMES texi2pdf PATHS $ENV{PATH})
set(TEXI2PDF ${TEXI2PDF} -b)

find_program(MAKEINFO NAMES makeinfo PATHS $ENV{PATH})
set(TEXI2HTML ${MAKEINFO} --html
  --css-include=${CMAKE_SOURCE_DIR}/marsyas.css)

find_program(DOXYGEN NAMES doxygen PATHS $ENV{PATH})
set(DOXY ${DOXYGEN}
  ${CMAKE_SOURCE_DIR}/../src/marsyas/Doxyfile)


## prep
make_directory(${CMAKE_BINARY_DIR}/out-www/)
set(OUT-WWW ${CMAKE_BINARY_DIR}/out-www/)
configure_file(index.html.in ${OUT-WWW}/index.html COPYONLY)

make_directory(${OUT-WWW}/manual/)
set(MANUAL_DIR ${OUT-WWW}/manual/)
# ugly, but works
make_directory(${MANUAL_DIR}/marsyas-user/)
make_directory(${MANUAL_DIR}/marsyas-devel/)
make_directory(${MANUAL_DIR}/marsyas-cookbook/)
make_directory(${MANUAL_DIR}/marsyas-user/images/)
make_directory(${MANUAL_DIR}/marsyas-devel/images/)
make_directory(${MANUAL_DIR}/marsyas-cookbook/images/)

file(GLOB PNG_IMAGES ${CMAKE_SOURCE_DIR}/images/*.png)
foreach(PNG_FILE ${PNG_IMAGES})
# ugly, but works
	configure_file(${PNG_FILE} ${MANUAL_DIR}/marsyas-user/images/ COPYONLY)
	configure_file(${PNG_FILE} ${MANUAL_DIR}/marsyas-devel/images/ COPYONLY)
	configure_file(${PNG_FILE} ${MANUAL_DIR}/marsyas-cookbook/images/ COPYONLY)
endforeach(PNG_FILE)


## build docs
add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-user.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-user.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-user/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-user.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-devel.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-devel.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-devel/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-devel.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-cookbook.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-cookbook.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-cookbook/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-cookbook.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${OUT-WWW}/sourceDoc/index.html
	COMMAND ${DOXY}
)

add_custom_target(doxy
	DEPENDS ${OUT-WWW}/sourceDoc/index.html
)

add_custom_target(docs ALL
	DEPENDS ${MANUAL_DIR}/marsyas-user.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-user/index.html
	DEPENDS ${MANUAL_DIR}/marsyas-devel.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-devel/index.html
	DEPENDS ${MANUAL_DIR}/marsyas-cookbook.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-cookbook/index.html
	DEPENDS ${OUT-WWW}/sourceDoc/index.html
)

file(COPY ${PNG_IMAGES} DESTINATION ${OUT-WWW}/marsyas-user/images/)
file(COPY ${CMAKE_SOURCE_DIR}/examples DESTINATION ${OUT-WWW}/manual)


