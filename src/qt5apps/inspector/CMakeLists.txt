set( inspector_moc_hdr
  #util/marsystem_model.h
  #util/tree_view.h
  main_controller.h
  gui/marsystem_view.h
  gui/controls_widget.h
  gui/realvec_widget.h

  ${CMAKE_SOURCE_DIR}/src/marsyasqt/Marx2DGraph.h
)

set( inspector_src
  main_qml.cpp
  main_controller.cpp
  #util/marsystem_model.cpp
  #util/tree_view.cpp
  gui/marsystem_view.cpp
  gui/controls_widget.cpp
  gui/realvec_widget.cpp

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
