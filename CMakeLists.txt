cmake_minimum_required(VERSION 3.20)

project(vmacro)

add_executable(vmacro src/instructions.c src/main.c src/utils.c)
target_link_libraries(vmacro pthread)