# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /root/test_project_src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/test_project_src/scripts

# Include any dependencies generated for this target.
include CMakeFiles/comparer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/comparer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/comparer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/comparer.dir/flags.make

CMakeFiles/comparer.dir/src/comparer/comparer.c.o: CMakeFiles/comparer.dir/flags.make
CMakeFiles/comparer.dir/src/comparer/comparer.c.o: ../src/comparer/comparer.c
CMakeFiles/comparer.dir/src/comparer/comparer.c.o: CMakeFiles/comparer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/test_project_src/scripts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/comparer.dir/src/comparer/comparer.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/comparer.dir/src/comparer/comparer.c.o -MF CMakeFiles/comparer.dir/src/comparer/comparer.c.o.d -o CMakeFiles/comparer.dir/src/comparer/comparer.c.o -c /root/test_project_src/src/comparer/comparer.c

CMakeFiles/comparer.dir/src/comparer/comparer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/comparer.dir/src/comparer/comparer.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/test_project_src/src/comparer/comparer.c > CMakeFiles/comparer.dir/src/comparer/comparer.c.i

CMakeFiles/comparer.dir/src/comparer/comparer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/comparer.dir/src/comparer/comparer.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/test_project_src/src/comparer/comparer.c -o CMakeFiles/comparer.dir/src/comparer/comparer.c.s

# Object files for target comparer
comparer_OBJECTS = \
"CMakeFiles/comparer.dir/src/comparer/comparer.c.o"

# External object files for target comparer
comparer_EXTERNAL_OBJECTS =

comparer: CMakeFiles/comparer.dir/src/comparer/comparer.c.o
comparer: CMakeFiles/comparer.dir/build.make
comparer: libbmp.so
comparer: CMakeFiles/comparer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/test_project_src/scripts/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable comparer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/comparer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/comparer.dir/build: comparer
.PHONY : CMakeFiles/comparer.dir/build

CMakeFiles/comparer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/comparer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/comparer.dir/clean

CMakeFiles/comparer.dir/depend:
	cd /root/test_project_src/scripts && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/test_project_src /root/test_project_src /root/test_project_src/scripts /root/test_project_src/scripts /root/test_project_src/scripts/CMakeFiles/comparer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/comparer.dir/depend
