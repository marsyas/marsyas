# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/george.tzanetakis/src/c++/marsyas_github

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/george.tzanetakis/src/c++/marsyas_github

# Include any dependencies generated for this target.
include src/apps/ibt/CMakeFiles/ibt.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/apps/ibt/CMakeFiles/ibt.dir/compiler_depend.make

# Include the progress variables for this target.
include src/apps/ibt/CMakeFiles/ibt.dir/progress.make

# Include the compile flags for this target's objects.
include src/apps/ibt/CMakeFiles/ibt.dir/flags.make

src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o: src/apps/ibt/CMakeFiles/ibt.dir/flags.make
src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o: src/apps/ibt/ibt.cpp
src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o: src/apps/ibt/CMakeFiles/ibt.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/george.tzanetakis/src/c++/marsyas_github/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o"
	cd /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o -MF CMakeFiles/ibt.dir/ibt.cpp.o.d -o CMakeFiles/ibt.dir/ibt.cpp.o -c /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt/ibt.cpp

src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ibt.dir/ibt.cpp.i"
	cd /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt/ibt.cpp > CMakeFiles/ibt.dir/ibt.cpp.i

src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ibt.dir/ibt.cpp.s"
	cd /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt/ibt.cpp -o CMakeFiles/ibt.dir/ibt.cpp.s

# Object files for target ibt
ibt_OBJECTS = \
"CMakeFiles/ibt.dir/ibt.cpp.o"

# External object files for target ibt
ibt_EXTERNAL_OBJECTS =

bin/ibt: src/apps/ibt/CMakeFiles/ibt.dir/ibt.cpp.o
bin/ibt: src/apps/ibt/CMakeFiles/ibt.dir/build.make
bin/ibt: lib/libmarsyas.dylib
bin/ibt: /opt/homebrew/lib/libsndfile.dylib
bin/ibt: lib/liboscpack.a
bin/ibt: src/apps/ibt/CMakeFiles/ibt.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/george.tzanetakis/src/c++/marsyas_github/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/ibt"
	cd /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ibt.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/apps/ibt/CMakeFiles/ibt.dir/build: bin/ibt
.PHONY : src/apps/ibt/CMakeFiles/ibt.dir/build

src/apps/ibt/CMakeFiles/ibt.dir/clean:
	cd /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt && $(CMAKE_COMMAND) -P CMakeFiles/ibt.dir/cmake_clean.cmake
.PHONY : src/apps/ibt/CMakeFiles/ibt.dir/clean

src/apps/ibt/CMakeFiles/ibt.dir/depend:
	cd /Users/george.tzanetakis/src/c++/marsyas_github && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/george.tzanetakis/src/c++/marsyas_github /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt /Users/george.tzanetakis/src/c++/marsyas_github /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt /Users/george.tzanetakis/src/c++/marsyas_github/src/apps/ibt/CMakeFiles/ibt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/apps/ibt/CMakeFiles/ibt.dir/depend

