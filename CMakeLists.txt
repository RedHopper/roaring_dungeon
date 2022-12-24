cmake_minimum_required(VERSION 3.0.0)
project(roaring_dungeon)
set(CMAKE_CXX_STANDART 11)
add_executable(roaring_dungeon
	src/main.cpp
	src/other_libs/glad.c)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(roaring_dungeon glfw OpenGL)
