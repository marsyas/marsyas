## global includes
set(APP_LIBS marsyas ${OS_LIBS} ${QT_LIBRARIES})
if(MARSYAS_OPENGL)
  set(APP_LIBS ${APP_LIBS} ${OPENGL_LIBRARIES})
endif()

include_directories(common)

## define macro for Qt4 apps
macro (marsyas_qt4app APP_NAME APP_SOURCES APP_HEADERS APP_UI APP_RESOURCES)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(APP_MOC_SOURCES ${APP_HEADERS})
qt4_wrap_ui(APP_UIHEADERS ${APP_UI})
qt4_add_resources(APP_RESOURCE_FILES ${APP_RESOURCES})

add_executable(${APP_NAME} ${APP_SOURCES} ${APP_UIHEADERS}
  ${APP_MOC_SOURCES} ${APP_RESOURCE_FILES})

target_link_Libraries(${APP_NAME} ${APP_LIBS})

# Do not install on Mac OS because of absolute paths to Qt dependencies
if(NOT APPLE)
  install(TARGETS ${APP_NAME} DESTINATION bin)
endif()

endmacro (marsyas_qt4app)

## build static lib and apps
if(MARSYAS_REALTIME)
  add_subdirectory(MarGrid)
endif()
