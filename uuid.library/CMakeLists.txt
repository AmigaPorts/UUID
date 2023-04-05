cmake_minimum_required(VERSION 3.14.0)
project(uuid VERSION 1.0.0) # Compat version with i2clib40 from aminet
get_verstring(VERSTRING)

add_link_options(-ffreestanding -nostdlib -nostartfiles -Wl,-e,__start)
add_compile_options(-Os -m68020 -fomit-frame-pointer)
add_compile_definitions(PRIVATE VERSION_STRING="${VERSTRING}")

add_library(uuid INTERFACE)
target_include_directories(uuid INTERFACE include)

add_executable(uuid.library
    src/uuid_init.c
    src/uuid_clear.c
    src/uuid_compare.c
    src/uuid_copy.c
    src/uuid_generate.c
    src/uuid_getnamespace.c
    src/uuid_pack.c
    src/uuid_unpack.c
    src/uuid_parse.c
    src/uuid_unparse.c
    src/sha1.c
    src/end.c
)

target_include_directories(uuid.library PRIVATE src)
target_link_libraries(uuid.library uuid)
target_compile_definitions(uuid.library PUBLIC
    LIBRARY_NAME="uuid.library"
    LIBRARY_VERSION=${PROJECT_VERSION_MAJOR}
    LIBRARY_REVISION=${PROJECT_VERSION_MINOR}
    LIBRARY_PRIORITY=0)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/uuid.library DESTINATION ./Libs/)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc DESTINATION ./Developer)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/sfd DESTINATION ./Developer)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ./Developer)
