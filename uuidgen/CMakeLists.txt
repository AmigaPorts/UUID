cmake_minimum_required(VERSION 3.14.0)
project(uuidgen VERSION 1.1.0) # Compat version with i2clib40 from aminet
get_verstring(VERSTRING)

add_link_options(-ffreestanding -nostdlib -nostartfiles -Wl,-e,__start)
add_compile_options(-Os -m68000 -fomit-frame-pointer)
add_compile_definitions(PRIVATE VERSION_STRING="${VERSTRING}")

add_executable(uuidgen
    src/uuidgen.c
)

target_link_libraries(uuidgen uuid)
set_target_properties(uuidgen PROPERTIES LINKER_LANGUAGE C)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/uuidgen DESTINATION ./C/)
