add_definitions(-DMARSYAS_VERSION=${MARSYAS_VERSION})

set(src marsyas-info.cpp)

marsyas_app(marsyas-info ${src})
