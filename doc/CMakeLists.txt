## find required programs
find_program(TEXI2PDF NAMES texi2pdf PATHS $ENV{PATH})
if(TEXI2PDF)
  set(TEXI2PDF ${TEXI2PDF} -b)
endif()

find_program(MAKEINFO NAMES makeinfo PATHS $ENV{PATH})
if(MAKEINFO)
  set(TEXI2HTML ${MAKEINFO} --html
    --css-include=${CMAKE_CURRENT_SOURCE_DIR}/marsyas.css)
endif()

find_program(DOXYGEN NAMES doxygen PATHS $ENV{PATH})

if(NOT TEXI2PDF OR NOT TEXI2HTML OR NOT DOXYGEN)
  message(STATUS "Documentation can not be built, because either texi2pdf, texi2html, or Doxygen is not installed.")
  return()
endif()

## prep
set(OUT-WWW ${CMAKE_CURRENT_BINARY_DIR}/out-www/)
make_directory(${OUT-WWW})
configure_file(index.html.in ${OUT-WWW}/index.html)

make_directory(${OUT-WWW}/manual/)
set(MANUAL_DIR ${OUT-WWW}/manual/)
# ugly, but works
make_directory(${MANUAL_DIR}/marsyas-user/)
make_directory(${MANUAL_DIR}/marsyas-devel/)
make_directory(${MANUAL_DIR}/marsyas-cookbook/)
make_directory(${MANUAL_DIR}/marsyas-user/images/)
make_directory(${MANUAL_DIR}/marsyas-devel/images/)
make_directory(${MANUAL_DIR}/marsyas-cookbook/images/)

file(GLOB PNG_IMAGES images/*.png)
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
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-user.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-user/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-user.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-devel.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-devel.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-devel/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-devel.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-cookbook.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-cookbook.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

add_custom_command(
	OUTPUT ${MANUAL_DIR}/marsyas-cookbook/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_CURRENT_SOURCE_DIR}/marsyas-cookbook.texi
	WORKING_DIRECTORY ${MANUAL_DIR}
)

configure_file( Doxyfile.in Doxyfile @ONLY )

make_directory(${OUT-WWW}/sourceDoc)

add_custom_command(
	OUTPUT ${OUT-WWW}/sourceDoc/index.html
	COMMAND ${DOXYGEN} ARGS Doxyfile
)

add_custom_target(doxy
	DEPENDS ${OUT-WWW}/sourceDoc/index.html
)

add_custom_target(docs
	DEPENDS ${MANUAL_DIR}/marsyas-user.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-user/index.html
	DEPENDS ${MANUAL_DIR}/marsyas-devel.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-devel/index.html
	DEPENDS ${MANUAL_DIR}/marsyas-cookbook.pdf
	DEPENDS ${MANUAL_DIR}/marsyas-cookbook/index.html
	DEPENDS ${OUT-WWW}/sourceDoc/index.html
)

file(COPY ${PNG_IMAGES} DESTINATION ${OUT-WWW}/marsyas-user/images/)
file(COPY examples DESTINATION ${OUT-WWW}/manual)


