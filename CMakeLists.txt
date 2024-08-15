cmake_minimum_required(VERSION 3.20)

project(vmacro)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

add_executable(vmacro src/instructions.c src/main.c src/utils.c src/window.c)
target_link_libraries(vmacro pthread PkgConfig::GTK4)