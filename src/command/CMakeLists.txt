file(GLOB_RECURSE ALL_SOURCE "*.cpp" "*.c")

add_library(commandlib STATIC ${ALL_SOURCE})