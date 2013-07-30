if(NOT MARSYAS_REALTIME)
  return()
endif()

qt5_wrap_cpp( marsyasqt5_moc_src
  ${CMAKE_SOURCE_DIR}/src/marsyasqt/marsystem_wrapper.h
)

add_library( marsyasqt5 STATIC
  ${CMAKE_SOURCE_DIR}/src/marsyasqt/marsystem_wrapper.cpp
  ${marsyasqt5_moc_src}
)

qt5_use_modules( marsyasqt5 Core )
