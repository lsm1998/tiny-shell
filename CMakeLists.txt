cmake_minimum_required(VERSION 3.10)

project(tiny-shell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} pthread srclib utilslib commandlib)