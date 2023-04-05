cmake_minimum_required(VERSION 3.14.0)
project(UUID VERSION 1.0.0)

include(cmake/verstring.cmake)
get_verstring(VERSTRING)

add_subdirectory(uuid.library)
add_subdirectory(uuidgen)
