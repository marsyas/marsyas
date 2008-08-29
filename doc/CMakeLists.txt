## global setup
cmake_minimum_required(VERSION 2.6)
project(Document)

find_program(TEXI2PDF NAMES texi2pdf PATHS $ENV{PATH})
set(TEXI2PDF ${TEXI2PDF} -b)
find_program(MAKEINFO NAMES makeinfo PATHS $ENV{PATH})
set(TEXI2HTML ${MAKEINFO} --html
  --css-include=${CMAKE_SOURCE_DIR}/marsyas.css)

file(GLOB PNG_IMAGES ${CMAKE_SOURCE_DIR}/images/*.png)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/marsyas-user.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-user.texi
)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/marsyas-user/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-user.texi
)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/marsyas-devel.pdf
	COMMAND ${TEXI2PDF}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-devel.texi
)

add_custom_command(
	OUTPUT ${CMAKE_BINARY_DIR}/marsyas-devel/index.html
	COMMAND ${TEXI2HTML}
	ARGS ${CMAKE_SOURCE_DIR}/marsyas-devel.texi
)

add_custom_target(marsyas-user ALL
	DEPENDS ${CMAKE_BINARY_DIR}/marsyas-user.pdf
	DEPENDS ${CMAKE_BINARY_DIR}/marsyas-user/index.html
	DEPENDS ${CMAKE_BINARY_DIR}/marsyas-devel.pdf
	DEPENDS ${CMAKE_BINARY_DIR}/marsyas-devel/index.html
)

# FIXME: do this without an install.
install(FILES ${PNG_IMAGES}
	DESTINATION ${CMAKE_BINARY_DIR}/marsyas-user/images/
)


