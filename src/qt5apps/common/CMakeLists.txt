set(src
  realvec_item_model.cpp
  realvec_table_widget.cpp
  control_model.cpp
)

set(moc_hdr
  realvec_table_widget.h
  control_model.h
)

if(MARSYAS_REALTIME)
  set(src
    ${src}
    marsystem_wrapper.cpp
  )
  set(moc_hdr
    ${moc_hdr}
    marsystem_wrapper.h
  )
endif()

qt5_wrap_cpp( moc_src ${moc_hdr})

add_library( marsyas-qt5 STATIC ${src} ${moc_src} )

target_link_libraries( marsyas-qt5 marsyas )

qt5_use_modules( marsyas-qt5 Core Widgets )

install( FILES
  marsyasqt_common.h
  marsystem_wrapper.h

  DESTINATION include/marsyas/qt5
)

install( TARGETS marsyas-qt5
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
