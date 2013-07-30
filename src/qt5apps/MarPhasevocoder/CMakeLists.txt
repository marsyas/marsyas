set(APP
  MarPhasevocoder
)

set(APP_SOURCES
  MarPhasevocoderWindow.cpp
  main.cpp
)

set(APP_HEADERS
  MarPhasevocoderWindow.h
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )

add_executable( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} )

target_link_libraries( ${APP} marsyas marsyasqt5 )
qt5_use_modules( ${APP} Widgets )

install(TARGETS ${APP} DESTINATION bin)
