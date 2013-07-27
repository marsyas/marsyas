cmake_minimum_required(VERSION 2.8.9)

find_package(Qt5Quick 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

if(WITH_CPP11)
  add_subdirectory(inspector)
endif()
