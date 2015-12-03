# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/marina/Documents/sdr1/gr-ccsds

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/marina/Documents/sdr1/gr-ccsds/build

# Include any dependencies generated for this target.
include apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/depend.make

# Include the progress variables for this target.
include apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/progress.make

# Include the compile flags for this target's objects.
include apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/flags.make

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/flags.make
apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o: ../apps/latency_measurements/ccsds_latency_receiver.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/marina/Documents/sdr1/gr-ccsds/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o"
	cd /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o -c /home/marina/Documents/sdr1/gr-ccsds/apps/latency_measurements/ccsds_latency_receiver.cpp

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.i"
	cd /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/marina/Documents/sdr1/gr-ccsds/apps/latency_measurements/ccsds_latency_receiver.cpp > CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.i

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.s"
	cd /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/marina/Documents/sdr1/gr-ccsds/apps/latency_measurements/ccsds_latency_receiver.cpp -o CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.s

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.requires:
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.requires

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.provides: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.requires
	$(MAKE) -f apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/build.make apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.provides.build
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.provides

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.provides.build: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o

# Object files for target ccsds_latency_receiver
ccsds_latency_receiver_OBJECTS = \
"CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o"

# External object files for target ccsds_latency_receiver
ccsds_latency_receiver_EXTERNAL_OBJECTS =

apps/latency_measurements/ccsds_latency_receiver: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o
apps/latency_measurements/ccsds_latency_receiver: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/build.make
apps/latency_measurements/ccsds_latency_receiver: /usr/lib/x86_64-linux-gnu/libboost_system.so
apps/latency_measurements/ccsds_latency_receiver: /usr/lib/x86_64-linux-gnu/libboost_thread.so
apps/latency_measurements/ccsds_latency_receiver: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ccsds_latency_receiver"
	cd /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ccsds_latency_receiver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/build: apps/latency_measurements/ccsds_latency_receiver
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/build

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/requires: apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/ccsds_latency_receiver.cpp.o.requires
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/requires

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/clean:
	cd /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements && $(CMAKE_COMMAND) -P CMakeFiles/ccsds_latency_receiver.dir/cmake_clean.cmake
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/clean

apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/depend:
	cd /home/marina/Documents/sdr1/gr-ccsds/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/marina/Documents/sdr1/gr-ccsds /home/marina/Documents/sdr1/gr-ccsds/apps/latency_measurements /home/marina/Documents/sdr1/gr-ccsds/build /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements /home/marina/Documents/sdr1/gr-ccsds/build/apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/latency_measurements/CMakeFiles/ccsds_latency_receiver.dir/depend

