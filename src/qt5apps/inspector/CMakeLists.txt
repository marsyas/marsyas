set( inspector_moc_hdr
  main_controller.h
  graph/marsystem_adaptor.h
  widgets/controls_widget.h
  widgets/realvec_widget.h

  ${CMAKE_SOURCE_DIR}/src/marsyasqt/Marx2DGraph.h
)

set( inspector_src
  main.cpp
  main_controller.cpp
  graph/marsyas_qml.cpp
  graph/marsystem_adaptor.cpp
  widgets/controls_widget.cpp
  widgets/realvec_widget.cpp

  ${CMAKE_SOURCE_DIR}/src/marsyasqt/Marx2DGraph.cpp
)

qt5_wrap_cpp( inspector_moc_src ${inspector_moc_hdr} )

add_executable( inspector ${inspector_src} ${inspector_moc_src})

include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/ )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyas/core )
include_directories( ${CMAKE_SOURCE_DIR}/src/marsyasqt/ )

qt5_use_modules( inspector Quick Widgets )

target_link_libraries( inspector marsyas )

set_target_properties( inspector PROPERTIES OUTPUT_NAME marsyas-inspector )
