set(APP
  MarPanning
)

set(APP_SOURCES
  main.cpp
  glwidget.cpp
  window.cpp
)

set(APP_HEADERS
  glwidget.h
  window.h
)

qt5_wrap_cpp( APP_MOC_SRC ${APP_HEADERS} )

marsyas_qt5_app( ${APP} ${APP_SOURCES} ${APP_MOC_SRC} )

qt5_use_modules( ${APP} Widgets OpenGL )
target_link_libraries( ${APP} ${OPENGL_LIBRARIES} )

if(APPLE)
  set(bundle_name "MarPanning")
  set(bundle_dir "${CMAKE_INSTALL_PREFIX}/${bundle_name}.app")

  set_target_properties( ${APP} PROPERTIES MACOSX_BUNDLE TRUE )
  set_target_properties( ${APP} PROPERTIES OUTPUT_NAME ${bundle_name} )

  install(TARGETS ${APP} BUNDLE DESTINATION .)

  install(
    FILES ${qt5_dir}/plugins/platforms/libqcocoa.dylib
    DESTINATION ${bundle_dir}/Contents/plugins/platforms
  )
  install(FILES qt.macos.conf DESTINATION ${bundle_dir}/Contents/Resources RENAME qt.conf)

  set(extra_fixup_items
    ${bundle_dir}/Contents/plugins/platforms/libqcocoa.dylib
  )

  install(CODE "
    include(BundleUtilities)
    fixup_bundle( \"${bundle_dir}\" \"${extra_fixup_items}\" \"${CMAKE_LIBRARY_PATH}\")
  ")

endif() #APPLE