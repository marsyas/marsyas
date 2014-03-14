set(app marsyas-script-cpp)
set(src
  script_to_cpp.cpp
)

add_executable(${app} ${src})
target_link_libraries(${app} marsyas)
install(TARGETS ${app} DESTINATION bin)
