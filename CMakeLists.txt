cmake_minimum_required(VERSION 3.0)
project(unpp2)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(utils SHARED ${PROJECT_SOURCE_DIR}/src/utils.c)

link_libraries(utils)

add_subdirectory(ch4)
add_subdirectory(ch5)
add_subdirectory(ch7)
add_subdirectory(ch8)

