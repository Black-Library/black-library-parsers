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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/changb/black-library/black-library-parsers

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/changb/black-library/black-library-parsers

# Include any dependencies generated for this target.
include src/CMakeFiles/Parsers.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/Parsers.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/Parsers.dir/flags.make

src/CMakeFiles/Parsers.dir/Parser.cc.o: src/CMakeFiles/Parsers.dir/flags.make
src/CMakeFiles/Parsers.dir/Parser.cc.o: src/Parser.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/Parsers.dir/Parser.cc.o"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Parsers.dir/Parser.cc.o -c /home/changb/black-library/black-library-parsers/src/Parser.cc

src/CMakeFiles/Parsers.dir/Parser.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parsers.dir/Parser.cc.i"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/changb/black-library/black-library-parsers/src/Parser.cc > CMakeFiles/Parsers.dir/Parser.cc.i

src/CMakeFiles/Parsers.dir/Parser.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parsers.dir/Parser.cc.s"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/changb/black-library/black-library-parsers/src/Parser.cc -o CMakeFiles/Parsers.dir/Parser.cc.s

src/CMakeFiles/Parsers.dir/Parser.cc.o.requires:

.PHONY : src/CMakeFiles/Parsers.dir/Parser.cc.o.requires

src/CMakeFiles/Parsers.dir/Parser.cc.o.provides: src/CMakeFiles/Parsers.dir/Parser.cc.o.requires
	$(MAKE) -f src/CMakeFiles/Parsers.dir/build.make src/CMakeFiles/Parsers.dir/Parser.cc.o.provides.build
.PHONY : src/CMakeFiles/Parsers.dir/Parser.cc.o.provides

src/CMakeFiles/Parsers.dir/Parser.cc.o.provides.build: src/CMakeFiles/Parsers.dir/Parser.cc.o


src/CMakeFiles/Parsers.dir/ParserAO3.cc.o: src/CMakeFiles/Parsers.dir/flags.make
src/CMakeFiles/Parsers.dir/ParserAO3.cc.o: src/ParserAO3.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/Parsers.dir/ParserAO3.cc.o"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Parsers.dir/ParserAO3.cc.o -c /home/changb/black-library/black-library-parsers/src/ParserAO3.cc

src/CMakeFiles/Parsers.dir/ParserAO3.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parsers.dir/ParserAO3.cc.i"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/changb/black-library/black-library-parsers/src/ParserAO3.cc > CMakeFiles/Parsers.dir/ParserAO3.cc.i

src/CMakeFiles/Parsers.dir/ParserAO3.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parsers.dir/ParserAO3.cc.s"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/changb/black-library/black-library-parsers/src/ParserAO3.cc -o CMakeFiles/Parsers.dir/ParserAO3.cc.s

src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.requires:

.PHONY : src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.requires

src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.provides: src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.requires
	$(MAKE) -f src/CMakeFiles/Parsers.dir/build.make src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.provides.build
.PHONY : src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.provides

src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.provides.build: src/CMakeFiles/Parsers.dir/ParserAO3.cc.o


src/CMakeFiles/Parsers.dir/ParserFactory.cc.o: src/CMakeFiles/Parsers.dir/flags.make
src/CMakeFiles/Parsers.dir/ParserFactory.cc.o: src/ParserFactory.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/Parsers.dir/ParserFactory.cc.o"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Parsers.dir/ParserFactory.cc.o -c /home/changb/black-library/black-library-parsers/src/ParserFactory.cc

src/CMakeFiles/Parsers.dir/ParserFactory.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parsers.dir/ParserFactory.cc.i"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/changb/black-library/black-library-parsers/src/ParserFactory.cc > CMakeFiles/Parsers.dir/ParserFactory.cc.i

src/CMakeFiles/Parsers.dir/ParserFactory.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parsers.dir/ParserFactory.cc.s"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/changb/black-library/black-library-parsers/src/ParserFactory.cc -o CMakeFiles/Parsers.dir/ParserFactory.cc.s

src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.requires:

.PHONY : src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.requires

src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.provides: src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.requires
	$(MAKE) -f src/CMakeFiles/Parsers.dir/build.make src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.provides.build
.PHONY : src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.provides

src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.provides.build: src/CMakeFiles/Parsers.dir/ParserFactory.cc.o


src/CMakeFiles/Parsers.dir/ParserManager.cc.o: src/CMakeFiles/Parsers.dir/flags.make
src/CMakeFiles/Parsers.dir/ParserManager.cc.o: src/ParserManager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object src/CMakeFiles/Parsers.dir/ParserManager.cc.o"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Parsers.dir/ParserManager.cc.o -c /home/changb/black-library/black-library-parsers/src/ParserManager.cc

src/CMakeFiles/Parsers.dir/ParserManager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parsers.dir/ParserManager.cc.i"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/changb/black-library/black-library-parsers/src/ParserManager.cc > CMakeFiles/Parsers.dir/ParserManager.cc.i

src/CMakeFiles/Parsers.dir/ParserManager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parsers.dir/ParserManager.cc.s"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/changb/black-library/black-library-parsers/src/ParserManager.cc -o CMakeFiles/Parsers.dir/ParserManager.cc.s

src/CMakeFiles/Parsers.dir/ParserManager.cc.o.requires:

.PHONY : src/CMakeFiles/Parsers.dir/ParserManager.cc.o.requires

src/CMakeFiles/Parsers.dir/ParserManager.cc.o.provides: src/CMakeFiles/Parsers.dir/ParserManager.cc.o.requires
	$(MAKE) -f src/CMakeFiles/Parsers.dir/build.make src/CMakeFiles/Parsers.dir/ParserManager.cc.o.provides.build
.PHONY : src/CMakeFiles/Parsers.dir/ParserManager.cc.o.provides

src/CMakeFiles/Parsers.dir/ParserManager.cc.o.provides.build: src/CMakeFiles/Parsers.dir/ParserManager.cc.o


src/CMakeFiles/Parsers.dir/SourceInformation.cc.o: src/CMakeFiles/Parsers.dir/flags.make
src/CMakeFiles/Parsers.dir/SourceInformation.cc.o: src/SourceInformation.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object src/CMakeFiles/Parsers.dir/SourceInformation.cc.o"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Parsers.dir/SourceInformation.cc.o -c /home/changb/black-library/black-library-parsers/src/SourceInformation.cc

src/CMakeFiles/Parsers.dir/SourceInformation.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Parsers.dir/SourceInformation.cc.i"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/changb/black-library/black-library-parsers/src/SourceInformation.cc > CMakeFiles/Parsers.dir/SourceInformation.cc.i

src/CMakeFiles/Parsers.dir/SourceInformation.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Parsers.dir/SourceInformation.cc.s"
	cd /home/changb/black-library/black-library-parsers/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/changb/black-library/black-library-parsers/src/SourceInformation.cc -o CMakeFiles/Parsers.dir/SourceInformation.cc.s

src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.requires:

.PHONY : src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.requires

src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.provides: src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.requires
	$(MAKE) -f src/CMakeFiles/Parsers.dir/build.make src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.provides.build
.PHONY : src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.provides

src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.provides.build: src/CMakeFiles/Parsers.dir/SourceInformation.cc.o


# Object files for target Parsers
Parsers_OBJECTS = \
"CMakeFiles/Parsers.dir/Parser.cc.o" \
"CMakeFiles/Parsers.dir/ParserAO3.cc.o" \
"CMakeFiles/Parsers.dir/ParserFactory.cc.o" \
"CMakeFiles/Parsers.dir/ParserManager.cc.o" \
"CMakeFiles/Parsers.dir/SourceInformation.cc.o"

# External object files for target Parsers
Parsers_EXTERNAL_OBJECTS =

src/libParsers.so: src/CMakeFiles/Parsers.dir/Parser.cc.o
src/libParsers.so: src/CMakeFiles/Parsers.dir/ParserAO3.cc.o
src/libParsers.so: src/CMakeFiles/Parsers.dir/ParserFactory.cc.o
src/libParsers.so: src/CMakeFiles/Parsers.dir/ParserManager.cc.o
src/libParsers.so: src/CMakeFiles/Parsers.dir/SourceInformation.cc.o
src/libParsers.so: src/CMakeFiles/Parsers.dir/build.make
src/libParsers.so: /usr/lib/x86_64-linux-gnu/libcurl.so
src/libParsers.so: /usr/lib/x86_64-linux-gnu/libxml2.so
src/libParsers.so: src/CMakeFiles/Parsers.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/changb/black-library/black-library-parsers/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library libParsers.so"
	cd /home/changb/black-library/black-library-parsers/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Parsers.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/Parsers.dir/build: src/libParsers.so

.PHONY : src/CMakeFiles/Parsers.dir/build

src/CMakeFiles/Parsers.dir/requires: src/CMakeFiles/Parsers.dir/Parser.cc.o.requires
src/CMakeFiles/Parsers.dir/requires: src/CMakeFiles/Parsers.dir/ParserAO3.cc.o.requires
src/CMakeFiles/Parsers.dir/requires: src/CMakeFiles/Parsers.dir/ParserFactory.cc.o.requires
src/CMakeFiles/Parsers.dir/requires: src/CMakeFiles/Parsers.dir/ParserManager.cc.o.requires
src/CMakeFiles/Parsers.dir/requires: src/CMakeFiles/Parsers.dir/SourceInformation.cc.o.requires

.PHONY : src/CMakeFiles/Parsers.dir/requires

src/CMakeFiles/Parsers.dir/clean:
	cd /home/changb/black-library/black-library-parsers/src && $(CMAKE_COMMAND) -P CMakeFiles/Parsers.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/Parsers.dir/clean

src/CMakeFiles/Parsers.dir/depend:
	cd /home/changb/black-library/black-library-parsers && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/changb/black-library/black-library-parsers /home/changb/black-library/black-library-parsers/src /home/changb/black-library/black-library-parsers /home/changb/black-library/black-library-parsers/src /home/changb/black-library/black-library-parsers/src/CMakeFiles/Parsers.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/Parsers.dir/depend

