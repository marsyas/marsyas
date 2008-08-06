set(APP sfinfo)

# you probably don't need to change anything below this
add_executable(${APP} ${APP}.cpp)
target_link_libraries(${APP} marsyas ${OS_LIBS})
install(TARGETS ${APP} DESTINATION bin)

