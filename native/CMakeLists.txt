cmake_minimum_required(VERSION 3.19)
project(native)
set(CMAKE_CXX_STANDARD 20)

find_package(JNI)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.*)

add_library(cio SHARED ${SRC} Source/systems/cio/ntos/fd_stream.cpp)
target_link_libraries(cio PRIVATE ${JNI_LIBRARIES})
target_include_directories(cio PRIVATE ${JNI_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/Source)