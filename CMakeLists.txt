cmake_minimum_required(VERSION 3.16)

project("ToyWebServer" VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(DBG_BUILD TRUE CACHE BOOL "To build in debug mode or not")

if (DBG_BUILD)
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -g -Og)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -O2)
endif()

# enable_testing()
include_directories(includes)
add_subdirectory(src)
# add_subdirectory(tests)
