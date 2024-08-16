cmake_minimum_required(VERSION 3.21)

project(vmacro)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(vmacro src/instructions.c src/main.c src/utils.c src/window.c)
set_property(TARGET vmacro PROPERTY C_STANDARD 23)
target_link_libraries(vmacro pthread PkgConfig::GTK4)