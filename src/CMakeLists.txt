add_subdirectory(command)
add_subdirectory(utils)

file(GLOB_RECURSE ALL_SOURCE "*.cpp" "*.c")

add_library(srclib STATIC ${ALL_SOURCE})