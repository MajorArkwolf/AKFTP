# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.17.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.17.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build

# Utility rule file for distcheck.

# Include the progress variables for this target.
include lib/json-c/CMakeFiles/distcheck.dir/progress.make

lib/json-c/CMakeFiles/distcheck:
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && make package_source
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && tar -xvf json-c-0.15.99-Source.tar.gz
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && mkdir json-c-0.15.99-Source/build
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && cmake json-c-0.15.99-Source/ -B"./json-c-0.15.99-Source/build/"
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && make -C json-c-0.15.99-Source/build
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && make test -C json-c-0.15.99-Source/build
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && rm -rf json-c-0.15.99-Source

distcheck: lib/json-c/CMakeFiles/distcheck
distcheck: lib/json-c/CMakeFiles/distcheck.dir/build.make

.PHONY : distcheck

# Rule to build all files generated by this target.
lib/json-c/CMakeFiles/distcheck.dir/build: distcheck

.PHONY : lib/json-c/CMakeFiles/distcheck.dir/build

lib/json-c/CMakeFiles/distcheck.dir/clean:
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c && $(CMAKE_COMMAND) -P CMakeFiles/distcheck.dir/cmake_clean.cmake
.PHONY : lib/json-c/CMakeFiles/distcheck.dir/clean

lib/json-c/CMakeFiles/distcheck.dir/depend:
	cd /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/lib/json-c /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c /Users/arkwolf/Documents/Coding/Schoolwork/ICT374-Assignment2-AKFTP/build/lib/json-c/CMakeFiles/distcheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/json-c/CMakeFiles/distcheck.dir/depend
