# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


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
CMAKE_COMMAND = /usr/local/cmake-3.10.0-Linux-x86_64/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake-3.10.0-Linux-x86_64/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dan/Server/src/qb/nanoraft/testraft

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dan/Server/src/qb/nanoraft/testraft/build

# Include any dependencies generated for this target.
include CMakeFiles/testraft.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testraft.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testraft.dir/flags.make

CMakeFiles/testraft.dir/main.cpp.o: CMakeFiles/testraft.dir/flags.make
CMakeFiles/testraft.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testraft.dir/main.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testraft.dir/main.cpp.o -c /home/dan/Server/src/qb/nanoraft/testraft/main.cpp

CMakeFiles/testraft.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testraft.dir/main.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dan/Server/src/qb/nanoraft/testraft/main.cpp > CMakeFiles/testraft.dir/main.cpp.i

CMakeFiles/testraft.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testraft.dir/main.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dan/Server/src/qb/nanoraft/testraft/main.cpp -o CMakeFiles/testraft.dir/main.cpp.s

CMakeFiles/testraft.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/testraft.dir/main.cpp.o.requires

CMakeFiles/testraft.dir/main.cpp.o.provides: CMakeFiles/testraft.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/testraft.dir/build.make CMakeFiles/testraft.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/testraft.dir/main.cpp.o.provides

CMakeFiles/testraft.dir/main.cpp.o.provides.build: CMakeFiles/testraft.dir/main.cpp.o


CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o: CMakeFiles/testraft.dir/flags.make
CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o: /home/dan/Server/src/qb/nanoraft/RaftProxy.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o -c /home/dan/Server/src/qb/nanoraft/RaftProxy.cpp

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dan/Server/src/qb/nanoraft/RaftProxy.cpp > CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.i

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dan/Server/src/qb/nanoraft/RaftProxy.cpp -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.s

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.requires:

.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.requires

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.provides: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.requires
	$(MAKE) -f CMakeFiles/testraft.dir/build.make CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.provides.build
.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.provides

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.provides.build: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o


CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o: CMakeFiles/testraft.dir/flags.make
CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o: /home/dan/Server/src/qb/nanoraft/RaftServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o -c /home/dan/Server/src/qb/nanoraft/RaftServer.cpp

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dan/Server/src/qb/nanoraft/RaftServer.cpp > CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.i

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dan/Server/src/qb/nanoraft/RaftServer.cpp -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.s

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.requires:

.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.requires

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.provides: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.requires
	$(MAKE) -f CMakeFiles/testraft.dir/build.make CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.provides.build
.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.provides

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.provides.build: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o


CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o: CMakeFiles/testraft.dir/flags.make
CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o: /home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o -c /home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp > CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.i

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp -o CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.s

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.requires:

.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.requires

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.provides: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.requires
	$(MAKE) -f CMakeFiles/testraft.dir/build.make CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.provides.build
.PHONY : CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.provides

CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.provides.build: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o


# Object files for target testraft
testraft_OBJECTS = \
"CMakeFiles/testraft.dir/main.cpp.o" \
"CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o" \
"CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o" \
"CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o"

# External object files for target testraft
testraft_EXTERNAL_OBJECTS =

bin/testraft: CMakeFiles/testraft.dir/main.cpp.o
bin/testraft: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o
bin/testraft: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o
bin/testraft: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o
bin/testraft: CMakeFiles/testraft.dir/build.make
bin/testraft: CMakeFiles/testraft.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable bin/testraft"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testraft.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testraft.dir/build: bin/testraft

.PHONY : CMakeFiles/testraft.dir/build

CMakeFiles/testraft.dir/requires: CMakeFiles/testraft.dir/main.cpp.o.requires
CMakeFiles/testraft.dir/requires: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftProxy.cpp.o.requires
CMakeFiles/testraft.dir/requires: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftServer.cpp.o.requires
CMakeFiles/testraft.dir/requires: CMakeFiles/testraft.dir/home/dan/Server/src/qb/nanoraft/RaftLogEntry.cpp.o.requires

.PHONY : CMakeFiles/testraft.dir/requires

CMakeFiles/testraft.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testraft.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testraft.dir/clean

CMakeFiles/testraft.dir/depend:
	cd /home/dan/Server/src/qb/nanoraft/testraft/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dan/Server/src/qb/nanoraft/testraft /home/dan/Server/src/qb/nanoraft/testraft /home/dan/Server/src/qb/nanoraft/testraft/build /home/dan/Server/src/qb/nanoraft/testraft/build /home/dan/Server/src/qb/nanoraft/testraft/build/CMakeFiles/testraft.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testraft.dir/depend

