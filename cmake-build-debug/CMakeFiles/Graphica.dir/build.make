# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/aarch64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Graphica.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Graphica.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Graphica.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Graphica.dir/flags.make

CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o: CMakeFiles/Graphica.dir/flags.make
CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o: /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/Main_Classes/main.cpp
CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o: CMakeFiles/Graphica.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o -MF CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o.d -o CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o -c /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/Main_Classes/main.cpp

CMakeFiles/Graphica.dir/Main_Classes/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Graphica.dir/Main_Classes/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/Main_Classes/main.cpp > CMakeFiles/Graphica.dir/Main_Classes/main.cpp.i

CMakeFiles/Graphica.dir/Main_Classes/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Graphica.dir/Main_Classes/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/Main_Classes/main.cpp -o CMakeFiles/Graphica.dir/Main_Classes/main.cpp.s

# Object files for target Graphica
Graphica_OBJECTS = \
"CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o"

# External object files for target Graphica
Graphica_EXTERNAL_OBJECTS =

Graphica: CMakeFiles/Graphica.dir/Main_Classes/main.cpp.o
Graphica: CMakeFiles/Graphica.dir/build.make
Graphica: CMakeFiles/Graphica.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Graphica"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Graphica.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Graphica.dir/build: Graphica
.PHONY : CMakeFiles/Graphica.dir/build

CMakeFiles/Graphica.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Graphica.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Graphica.dir/clean

CMakeFiles/Graphica.dir/depend:
	cd /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug /Users/aryan_singh/Desktop/Personal_Projects/Graphica/Graphica/cmake-build-debug/CMakeFiles/Graphica.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Graphica.dir/depend

