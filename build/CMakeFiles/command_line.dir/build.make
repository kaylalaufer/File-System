# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build"

# Include any dependencies generated for this target.
include CMakeFiles/command_line.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/command_line.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/command_line.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/command_line.dir/flags.make

CMakeFiles/command_line.dir/codegen:
.PHONY : CMakeFiles/command_line.dir/codegen

CMakeFiles/command_line.dir/command_line.cpp.o: CMakeFiles/command_line.dir/flags.make
CMakeFiles/command_line.dir/command_line.cpp.o: /Users/kaylalaufer/Documents/Fordham/Operating\ Systems/Final_Project/command_line.cpp
CMakeFiles/command_line.dir/command_line.cpp.o: CMakeFiles/command_line.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/command_line.dir/command_line.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/command_line.dir/command_line.cpp.o -MF CMakeFiles/command_line.dir/command_line.cpp.o.d -o CMakeFiles/command_line.dir/command_line.cpp.o -c "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/command_line.cpp"

CMakeFiles/command_line.dir/command_line.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/command_line.dir/command_line.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/command_line.cpp" > CMakeFiles/command_line.dir/command_line.cpp.i

CMakeFiles/command_line.dir/command_line.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/command_line.dir/command_line.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/command_line.cpp" -o CMakeFiles/command_line.dir/command_line.cpp.s

CMakeFiles/command_line.dir/file_manager.cpp.o: CMakeFiles/command_line.dir/flags.make
CMakeFiles/command_line.dir/file_manager.cpp.o: /Users/kaylalaufer/Documents/Fordham/Operating\ Systems/Final_Project/file_manager.cpp
CMakeFiles/command_line.dir/file_manager.cpp.o: CMakeFiles/command_line.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/command_line.dir/file_manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/command_line.dir/file_manager.cpp.o -MF CMakeFiles/command_line.dir/file_manager.cpp.o.d -o CMakeFiles/command_line.dir/file_manager.cpp.o -c "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/file_manager.cpp"

CMakeFiles/command_line.dir/file_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/command_line.dir/file_manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/file_manager.cpp" > CMakeFiles/command_line.dir/file_manager.cpp.i

CMakeFiles/command_line.dir/file_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/command_line.dir/file_manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/file_manager.cpp" -o CMakeFiles/command_line.dir/file_manager.cpp.s

CMakeFiles/command_line.dir/disk_manager.cpp.o: CMakeFiles/command_line.dir/flags.make
CMakeFiles/command_line.dir/disk_manager.cpp.o: /Users/kaylalaufer/Documents/Fordham/Operating\ Systems/Final_Project/disk_manager.cpp
CMakeFiles/command_line.dir/disk_manager.cpp.o: CMakeFiles/command_line.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/command_line.dir/disk_manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/command_line.dir/disk_manager.cpp.o -MF CMakeFiles/command_line.dir/disk_manager.cpp.o.d -o CMakeFiles/command_line.dir/disk_manager.cpp.o -c "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/disk_manager.cpp"

CMakeFiles/command_line.dir/disk_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/command_line.dir/disk_manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/disk_manager.cpp" > CMakeFiles/command_line.dir/disk_manager.cpp.i

CMakeFiles/command_line.dir/disk_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/command_line.dir/disk_manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/disk_manager.cpp" -o CMakeFiles/command_line.dir/disk_manager.cpp.s

# Object files for target command_line
command_line_OBJECTS = \
"CMakeFiles/command_line.dir/command_line.cpp.o" \
"CMakeFiles/command_line.dir/file_manager.cpp.o" \
"CMakeFiles/command_line.dir/disk_manager.cpp.o"

# External object files for target command_line
command_line_EXTERNAL_OBJECTS =

command_line: CMakeFiles/command_line.dir/command_line.cpp.o
command_line: CMakeFiles/command_line.dir/file_manager.cpp.o
command_line: CMakeFiles/command_line.dir/disk_manager.cpp.o
command_line: CMakeFiles/command_line.dir/build.make
command_line: CMakeFiles/command_line.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable command_line"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/command_line.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/command_line.dir/build: command_line
.PHONY : CMakeFiles/command_line.dir/build

CMakeFiles/command_line.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/command_line.dir/cmake_clean.cmake
.PHONY : CMakeFiles/command_line.dir/clean

CMakeFiles/command_line.dir/depend:
	cd "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project" "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project" "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build" "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build" "/Users/kaylalaufer/Documents/Fordham/Operating Systems/Final_Project/build/CMakeFiles/command_line.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/command_line.dir/depend

