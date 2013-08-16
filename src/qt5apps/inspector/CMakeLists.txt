set( inspector_moc_hdr
  main.h
  debug_controller.h
  graph/marsystem_adaptor.h
  widgets/controls_widget.h
  widgets/realvec_widget.h
  widgets/debug_widget.h
)

set( inspector_src
  main.cpp
  debug_controller.cpp
  graph/marsyas_qml.cpp
  graph/marsystem_adaptor.cpp
  widgets/controls_widget.cpp
  widgets/realvec_widget.cpp
  widgets/debug_widget.cpp
)

qt5_wrap_cpp( inspector_moc_src ${inspector_moc_hdr} )

qt5_add_resources( inspector_resources_src graph/qml/resources.qrc )

add_executable( inspector
  ${inspector_src}
  ${inspector_moc_src}
  ${inspector_resources_src}
)

include_directories( ${CMAKE_SOURCE_DIR}/src )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/core )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyasqt/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/otherlibs/qwt )

qt5_use_modules( inspector Quick Widgets )

target_link_libraries( inspector marsyas marsyasqt5 qwt )

set_target_properties( inspector PROPERTIES OUTPUT_NAME marsyas-inspector )
