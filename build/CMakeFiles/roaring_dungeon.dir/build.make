# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/oyne/Documents/cmpire/roaring_dungeon

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/oyne/Documents/cmpire/roaring_dungeon/build

# Include any dependencies generated for this target.
include CMakeFiles/roaring_dungeon.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/roaring_dungeon.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/roaring_dungeon.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/roaring_dungeon.dir/flags.make

CMakeFiles/roaring_dungeon.dir/src/main.cpp.o: CMakeFiles/roaring_dungeon.dir/flags.make
CMakeFiles/roaring_dungeon.dir/src/main.cpp.o: /home/oyne/Documents/cmpire/roaring_dungeon/src/main.cpp
CMakeFiles/roaring_dungeon.dir/src/main.cpp.o: CMakeFiles/roaring_dungeon.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/oyne/Documents/cmpire/roaring_dungeon/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/roaring_dungeon.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/roaring_dungeon.dir/src/main.cpp.o -MF CMakeFiles/roaring_dungeon.dir/src/main.cpp.o.d -o CMakeFiles/roaring_dungeon.dir/src/main.cpp.o -c /home/oyne/Documents/cmpire/roaring_dungeon/src/main.cpp

CMakeFiles/roaring_dungeon.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/roaring_dungeon.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/oyne/Documents/cmpire/roaring_dungeon/src/main.cpp > CMakeFiles/roaring_dungeon.dir/src/main.cpp.i

CMakeFiles/roaring_dungeon.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/roaring_dungeon.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/oyne/Documents/cmpire/roaring_dungeon/src/main.cpp -o CMakeFiles/roaring_dungeon.dir/src/main.cpp.s

CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o: CMakeFiles/roaring_dungeon.dir/flags.make
CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o: /home/oyne/Documents/cmpire/roaring_dungeon/src/other_libs/glad.c
CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o: CMakeFiles/roaring_dungeon.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/oyne/Documents/cmpire/roaring_dungeon/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o -MF CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o.d -o CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o -c /home/oyne/Documents/cmpire/roaring_dungeon/src/other_libs/glad.c

CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/oyne/Documents/cmpire/roaring_dungeon/src/other_libs/glad.c > CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.i

CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/oyne/Documents/cmpire/roaring_dungeon/src/other_libs/glad.c -o CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.s

# Object files for target roaring_dungeon
roaring_dungeon_OBJECTS = \
"CMakeFiles/roaring_dungeon.dir/src/main.cpp.o" \
"CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o"

# External object files for target roaring_dungeon
roaring_dungeon_EXTERNAL_OBJECTS =

roaring_dungeon: CMakeFiles/roaring_dungeon.dir/src/main.cpp.o
roaring_dungeon: CMakeFiles/roaring_dungeon.dir/src/other_libs/glad.c.o
roaring_dungeon: CMakeFiles/roaring_dungeon.dir/build.make
roaring_dungeon: /usr/local/lib/libglfw3.a
roaring_dungeon: /usr/lib/librt.a
roaring_dungeon: /usr/lib/libm.so
roaring_dungeon: CMakeFiles/roaring_dungeon.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/oyne/Documents/cmpire/roaring_dungeon/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable roaring_dungeon"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/roaring_dungeon.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/roaring_dungeon.dir/build: roaring_dungeon
.PHONY : CMakeFiles/roaring_dungeon.dir/build

CMakeFiles/roaring_dungeon.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/roaring_dungeon.dir/cmake_clean.cmake
.PHONY : CMakeFiles/roaring_dungeon.dir/clean

CMakeFiles/roaring_dungeon.dir/depend:
	cd /home/oyne/Documents/cmpire/roaring_dungeon/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/oyne/Documents/cmpire/roaring_dungeon /home/oyne/Documents/cmpire/roaring_dungeon /home/oyne/Documents/cmpire/roaring_dungeon/build /home/oyne/Documents/cmpire/roaring_dungeon/build /home/oyne/Documents/cmpire/roaring_dungeon/build/CMakeFiles/roaring_dungeon.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/roaring_dungeon.dir/depend

