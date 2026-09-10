cmake_minimum_required(VERSION 3.5)

option(BUILD_TESTS "Build test programs" OFF)

project(bcrypt)

set(CMAKE_CXX_STANDARD 11)

add_library(bcrypt src/bcrypt.cpp src/blowfish.cpp)
target_include_directories(bcrypt PRIVATE src/)
target_include_directories(bcrypt PUBLIC include/)

if (WIN32)
  target_compile_definitions(bcrypt PRIVATE _WIN32_WINNT=0x0601 WINVER=0x0601)
endif()

if(BUILD_TESTS)
  add_executable(test test/main.cpp)
  target_link_libraries(test PRIVATE bcrypt)
endif()
