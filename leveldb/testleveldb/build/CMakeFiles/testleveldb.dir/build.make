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
CMAKE_SOURCE_DIR = /home/dan/Server/src/qb/raft_server/leveldb/testleveldb

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build

# Include any dependencies generated for this target.
include CMakeFiles/testleveldb.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/testleveldb.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testleveldb.dir/flags.make

CMakeFiles/testleveldb.dir/main.cpp.o: CMakeFiles/testleveldb.dir/flags.make
CMakeFiles/testleveldb.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testleveldb.dir/main.cpp.o"
	g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/testleveldb.dir/main.cpp.o -c /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/main.cpp

CMakeFiles/testleveldb.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/testleveldb.dir/main.cpp.i"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/main.cpp > CMakeFiles/testleveldb.dir/main.cpp.i

CMakeFiles/testleveldb.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/testleveldb.dir/main.cpp.s"
	g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/main.cpp -o CMakeFiles/testleveldb.dir/main.cpp.s

CMakeFiles/testleveldb.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/testleveldb.dir/main.cpp.o.requires

CMakeFiles/testleveldb.dir/main.cpp.o.provides: CMakeFiles/testleveldb.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/testleveldb.dir/build.make CMakeFiles/testleveldb.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/testleveldb.dir/main.cpp.o.provides

CMakeFiles/testleveldb.dir/main.cpp.o.provides.build: CMakeFiles/testleveldb.dir/main.cpp.o


# Object files for target testleveldb
testleveldb_OBJECTS = \
"CMakeFiles/testleveldb.dir/main.cpp.o"

# External object files for target testleveldb
testleveldb_EXTERNAL_OBJECTS =

bin/testleveldb: CMakeFiles/testleveldb.dir/main.cpp.o
bin/testleveldb: CMakeFiles/testleveldb.dir/build.make
bin/testleveldb: /home/dan/Server/src/qb/raft_server/lib/libleveldb.a
bin/testleveldb: CMakeFiles/testleveldb.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/testleveldb"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testleveldb.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testleveldb.dir/build: bin/testleveldb

.PHONY : CMakeFiles/testleveldb.dir/build

CMakeFiles/testleveldb.dir/requires: CMakeFiles/testleveldb.dir/main.cpp.o.requires

.PHONY : CMakeFiles/testleveldb.dir/requires

CMakeFiles/testleveldb.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testleveldb.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testleveldb.dir/clean

CMakeFiles/testleveldb.dir/depend:
	cd /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dan/Server/src/qb/raft_server/leveldb/testleveldb /home/dan/Server/src/qb/raft_server/leveldb/testleveldb /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build /home/dan/Server/src/qb/raft_server/leveldb/testleveldb/build/CMakeFiles/testleveldb.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/testleveldb.dir/depend

