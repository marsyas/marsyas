if(MARSYAS_JSON)
  add_definitions(-DMARSYAS_HAS_JSON)
endif()

if(MARSYAS_SCRIPT)
  add_definitions(-DMARSYAS_HAS_SCRIPT)
endif()


set( inspector_moc_hdr
  main.h
  debug_controller.h
  graph/marsystem_adaptor.h
  graph/connection_item.h
  widgets/controls_widget.h
  widgets/realvec_widget.h
  widgets/stats_widget.h
)

set( inspector_src
  main.cpp
  debug_controller.cpp
  graph/marsyas_qml.cpp
  graph/marsystem_adaptor.cpp
  graph/connection_item.cpp
  widgets/controls_widget.cpp
  widgets/realvec_widget.cpp
  widgets/stats_widget.cpp
)

qt5_wrap_cpp( inspector_moc_src ${inspector_moc_hdr} )

qt5_add_resources( inspector_resources_src graph/qml/resources.qrc )

marsyas_qt5_app( inspector
  ${inspector_src}
  ${inspector_moc_src}
  ${inspector_resources_src}
)

include_directories( ${CMAKE_SOURCE_DIR}/src/otherlibs/qwt )

qt5_use_modules( inspector Quick Widgets )

target_link_libraries( inspector qwt )

set_target_properties( inspector PROPERTIES OUTPUT_NAME marsyas-inspector )

if(APPLE)
  set(bundle_name "Marsyas Inspector")
  set(bundle_dir "${CMAKE_INSTALL_PREFIX}/${bundle_name}.app")

  set_target_properties( inspector PROPERTIES MACOSX_BUNDLE TRUE )
  set_target_properties( inspector PROPERTIES OUTPUT_NAME ${bundle_name} )

  install(TARGETS inspector BUNDLE DESTINATION .)

  install(
    FILES ${qt5_dir}/plugins/platforms/libqcocoa.dylib
    DESTINATION ${bundle_dir}/Contents/plugins/platforms
  )
  install(DIRECTORY ${qt5_dir}/qml/QtQuick.2 DESTINATION ${bundle_dir}/Contents/qml)
  install(DIRECTORY ${qt5_dir}/qml/QtQuick/Layouts DESTINATION ${bundle_dir}/Contents/qml/QtQuick)
  install(FILES qt.macos.conf DESTINATION ${bundle_dir}/Contents/Resources RENAME qt.conf)

  set(extra_fixup_items
    ${bundle_dir}/Contents/plugins/platforms/libqcocoa.dylib
    ${bundle_dir}/Contents/qml/QtQuick.2/libqtquick2plugin.dylib
    ${bundle_dir}/Contents/qml/QtQuick.2/libqtquick2plugin_debug.dylib
    ${bundle_dir}/Contents/qml/QtQuick/Layouts/libqquicklayoutsplugin.dylib
    ${bundle_dir}/Contents/qml/QtQuick/Layouts/libqquicklayoutsplugin_debug.dylib
  )

  install(CODE "
include(BundleUtilities)
fixup_bundle(
  \"${bundle_dir}\"
  \"${extra_fixup_items}\"
  \"${CMAKE_LIBRARY_PATH}\"
)
")

endif()
