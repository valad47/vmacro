cmake_minimum_required(VERSION 3.21)

project(vmacro)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#Commented until add actual visual interface
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
#target_link_libraries(vmacro pthread PkgConfig::GTK4)

add_executable(vmacro
	src/instructions.c
	src/main.c src/utils.c
#	src/window.c
	src/parsekey.c
	src/debug.c
	src/label.c
)
set_property(TARGET vmacro PROPERTY C_STANDARD 23)
