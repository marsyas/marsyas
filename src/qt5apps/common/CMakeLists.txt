set(src
  realvec_item_model.cpp
  realvec_table_widget.cpp
)

set(moc_hdr
  realvec_table_widget.h
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

add_library( marsyasqt5 STATIC ${src} ${moc_src} )

target_link_libraries( marsyasqt5 marsyas )

qt5_use_modules( marsyasqt5 Core Widgets )
