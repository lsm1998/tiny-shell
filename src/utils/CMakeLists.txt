file(GLOB_RECURSE ALL_SOURCE "*.cpp" "*.c")

add_library(utilslib STATIC ${ALL_SOURCE})