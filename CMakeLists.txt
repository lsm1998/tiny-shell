cmake_minimum_required(VERSION 3.10)

project(tiny-shell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_subdirectory(src)

add_executable(tiny-shell main.cpp src/tiny_shell.cpp src/utils/system_api.cpp)

target_link_libraries(${PROJECT_NAME} pthread)