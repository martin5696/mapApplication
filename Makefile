#################################################
# Configuration
##################################################
#Default build type
CONF=debug

#
# Tool names
#

#The name of the compiler to use
CXX = g++

#Name of the archiver used to create .a from sets of .o files
AR = ar

#
# Directories
#

#Directory for build temporaries
BUILD_DIR = build

#What directory contains the source files for the main executable?
EXE_SRC_DIR = main/src

#What directory contains the source files for the streets database static library?
LIB_STREETSDATABASE_SRC_DIR = libstreetsdatabase/src

#What directory contains the source files for the street map static library?
LIB_STREETMAP_SRC_DIR = libstreetmap/src

#What directory contains the source files for the street map library tests?
LIB_STREETMAP_TEST_DIR = libstreetmap/tests

#Global directory to look for custom library builds
ECE297_LIB_DIR = /cad2/ece297s/public/lib

#
# External Library Configuration
#

#Boost serialization
BOOST_SERIALIZATION_RELEASE_LIB := -lboost_serialization
BOOST_SERIALIZATION_DEBUG_LIB := $(ECE297_LIB_DIR)/debug/libboost_serialization.a
BOOST_SERIALIZATION_LIB := $(BOOST_SERIALIZATION_RELEASE_LIB) #Defines the version actually used, default release

#Unittest++
UNITTESTPP_RELEASE_LIB := -lunittest++
UNITTESTPP_DEBUG_LIB := $(ECE297_LIB_DIR)/debug/libunittest++.a
UNITTESTPP_LIB := $(UNITTESTPP_RELEASE_LIB) #Defines the version actually used, default release

#
# EasyGL Configuration
#
GRAPHICS_LIBS = -lX11 -lXft -lfontconfig
GRAPHICS_FLAGS = $(shell pkg-config --cflags freetype2) # evaluates to the correct include flags for the freetype headers


#
# Compiler flags
#

#What warning flags should be passed to the compiler?
WARN_FLAGS = -Wall

#What include flags should be passed to the compiler?
INCLUDE_FLAGS = -I$(LIB_STREETMAP_SRC_DIR) -I$(LIB_STREETSDATABASE_SRC_DIR) -I$(EXE_SRC_DIR) $(GRAPHICS_FLAGS) -I/opt/X11/include

#What options to generate header dependency files should be passed to the compiler?
DEP_FLAGS = -MMD -MP

#What extra flags to use in a debug build?
# Defining _GLIBCXX_DEBUG enables vector [] bounds checking
DEBUG_FLAGS = -g -O0 -D_GLIBCXX_DEBUG

#What extra flags to use in a release build?
RELEASE_FLAGS = -O3

#What extra flags to use in debug profile build?
DEBUG_PROFILE_FLAGS = $(DEBUG_FLAGS) -pg

#What extra flags to use in release profile build?
# Note: provide -g so symbols are included in profiling info
RELEASE_PROFILE_FLAGS = $(RELEASE_FLAGS) -g -pg

#Pick either debug/release/profile build flags 
ifeq (debug, $(CONF))
DEBUG_RELEASE_FLAGS := $(DEBUG_FLAGS)
else ifeq (release, $(CONF))
DEBUG_RELEASE_FLAGS := $(RELEASE_FLAGS)
else ifeq (debug_profile, $(CONF))
DEBUG_RELEASE_FLAGS := $(DEBUG_PROFILE_FLAGS)
else ifeq (release_profile, $(CONF))
DEBUG_RELEASE_FLAGS := $(RELEASE_PROFILE_FLAGS)
else
$(error Invalid value for CONF: '$(CONF)', must be 'debug', 'release', 'debug_profile', or 'release_profile'. Try 'make help' for usage)
endif

$(info Performing a '$(CONF)' build)

#Collect all the options to give to the compiler
CFLAGS = $(DEP_FLAGS) $(WARN_FLAGS) $(DEBUG_RELEASE_FLAGS) $(INCLUDE_FLAGS) --std=c++11 -pthread -fopenmp

#Choose the correct external library builds.
#If we are using a debug build AND we have a separately compiled debug 
#build of a library use it instead of the system default
ifeq (debug, $(findstring debug,$(CONF))) #Doing a debug build?

#Boost serialization
ifneq (, $(wildcard $(BOOST_SERIALIZATION_DEBUG_LIB))) #Debug lib file exists
BOOST_SERIALIZATION_LIB := $(BOOST_SERIALIZATION_DEBUG_LIB) #Use debug version
endif

#Unittest++
ifneq (, $(wildcard $(UNITTESTPP_DEBUG_LIB))) #Debug lib file exits
UNITTESTPP_LIB := $(UNITTESTPP_DEBUG_LIB) #Use debug version
endif

endif

#$(info unittest++ lib: $(UNITTESTPP_LIB))
#$(info boost_serialziation lib: $(BOOST_SERIALIZATION_LIB))

#Flags for linking
# -L. tells the linker to also look in the current directory
LFLAGS = -L. -L/opt/x11/lib -L/usr/local/lib/boost -L/usr/lib/x86_64-linux-gnu
LFLAGS_LIB_STREETSDATABASE = -lstreetsdatabase
LFLAGS_LIB_STREETMAP = -lstreetmap
OTHER_LIBS = -lreadline -pthread $(BOOST_SERIALIZATION_LIB)

#
# Archiver flags
#

#Flags for the archiver (used to create static libraries)
ARFLAGS = rvs

#
#Output files
#

#Name of the primary executable
EXE=mapper

#Name of the test executable
LIB_STREETMAP_TEST=test_libstreetmap

#Name of the street map static library
LIB_STREETMAP=libstreetmap.a

#Name of the streets database static library
LIB_STREETSDATABASE=libstreetsdatabase.a

#
# Generate object file names from source file names
#

#Objects associated with the main executable
EXE_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/$(CONF)/%.o,$(wildcard $(EXE_SRC_DIR)/*.cpp))

#Objects associated with the streets database library
LIB_STREETSDATABASE_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/$(CONF)/%.o,$(wildcard $(LIB_STREETSDATABASE_SRC_DIR)/*.cpp))

#Objects associated with the street map library
LIB_STREETMAP_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/$(CONF)/%.o,$(wildcard $(LIB_STREETMAP_SRC_DIR)/*.cpp))

#Objects associated with tests for the street map library
LIB_STREETMAP_TEST_OBJ=$(patsubst %.cpp, $(BUILD_DIR)/$(CONF)/%.o,$(wildcard $(LIB_STREETMAP_TEST_DIR)/*.cpp))

#
# Dependency files
#

#To capture dependencies on header files,
# we told the compiler to generate dependency 
# files associated with each object file
#
#The ':.o=.d' syntax means replace each filename ending in .o with .d
# For example:
#   build/main/main.o would become build/main/main.d
DEP = $(EXE_OBJ:.o=.d) $(LIB_STREETMAP_OBJ:.o=.d) $(LIB_STREETSDATABASE_OBJ:.o=.d) $(LIB_STREETMAP_TEST_OBJ:.o=.d) 

#Phony targets are always run (i.e. are always out of date)
#  We mark all executables and static libraries as phony so that they will
#  always be re-built. This ensures that all final libraries/executables 
#  will be of the same build CONF. This is important since using _GLIBCXX_DEBUG 
#  can cause the debug and release builds to be binary incompatible, causing odd 
#  errors if both debug and release components are mixed.
.PHONY: clean $(EXE) $(LIB_STREETMAP_TEST) $(LIB_STREETMAP) $(LIB_STREETSDATABASE)

##################################################
# Make targets
##################################################

#The default target
# This is called when you type 'make' on the command line
all: $(EXE)

#This runs the unit test executable
test: $(LIB_STREETMAP_TEST)
	@echo ""
	@echo "Running Unit Tests..."
	./$(LIB_STREETMAP_TEST)

#Include header file dependencies generated by a
# previous compile
-include $(DEP)

#Link main executable
$(EXE): $(EXE_OBJ) $(LIB_STREETMAP) $(LIB_STREETSDATABASE)
	$(CXX) $(CFLAGS) $^ $(LFLAGS) $(LFLAGS_LIB_STREETSDATABASE) $(LFLAGS_LIB_STREETMAP) $(GRAPHICS_LIBS) $(OTHER_LIBS) -o $@

#Link test executable
$(LIB_STREETMAP_TEST): $(LIB_STREETMAP_TEST_OBJ) $(LIB_STREETMAP) $(LIB_STREETSDATABASE)
	$(CXX) $(CFLAGS) $^ $(LFLAGS) $(LFLAGS_LIB_STREETSDATABASE) $(LFLAGS_LIB_STREETMAP) $(GRAPHICS_LIBS) $(OTHER_LIBS) $(UNITTESTPP_LIB) -o $@

#Street Map static library
$(LIB_STREETMAP): $(LIB_STREETMAP_OBJ)
	$(AR) $(ARFLAGS) $@ $^

#Streets Database static library
$(LIB_STREETSDATABASE): $(LIB_STREETSDATABASE_OBJ) 
	$(AR) $(ARFLAGS) $@ $^

#Note: % matches recursively between prefix and suffix
#      so %.cpp would match both src/a/a.cpp
#      and src/b/b.cpp
$(BUILD_DIR)/$(CONF)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(EXE) $(LIB_STREETMAP) $(LIB_STREETSDATABASE) $(LIB_STREETMAP_TEST)

help:
	@echo "Makefile for ECE297"
	@echo ""
	@echo "Usage: "
	@echo '    > make'
	@echo "        Call the default make target (all)."
	@echo "        This builds the project executable: '$(EXE)'."
	@echo "    > make clean"
	@echo "        Removes any generated files including exectuables,"
	@echo "        static libraries, and object files."
	@echo "    > make test"
	@echo "        Runs unit tests."
	@echo "        Builds and runs any tests found in $(LIB_STREETMAP_TEST_DIR),"
	@echo "        generating the test executable '$(LIB_STREETMAP_TEST)'."
	@echo "    > make help"
	@echo "        Prints this help message."
	@echo ""
	@echo ""
	@echo "Configuration Variables: "
	@echo "    CONF={debug | release | debug_profile | release_profile}"
	@echo "        Controls whether the build performs compiler optimizations"
	@echo "        to improve performance. Currently set to '$(CONF)'."
	@echo ""
	@echo "        With CONF=debug debugging symbols are turned on,"
	@echo "        compiler optimization is disabled, and extra checking"
	@echo "        in the standard library is enabled ($(DEBUG_FLAGS))."
	@echo ""
	@echo "        With CONF=release compiler optimization is enabled ($(RELEASE_FLAGS))."
	@echo ""
	@echo "        With CONF=debug_profile a debug build is created"
	@echo "        with gprof profiling enabled ($(DEBUG_PROFILE_FLAGS))."
	@echo ""
	@echo "        With CONF=release_profile a release build is created with"
	@echo "        debug symbols and gprof profiling enabled ($(RELEASE_PROFILE_FLAGS))."
	@echo ""
	@echo "        You can configure specify this option on the command line."
	@echo "        To perform a debug build you can use: "
	@echo "            > make CONF=debug"
	@echo "        To perform a release build you can use: "
	@echo "            > make CONF=release"
	@echo "        To perform a debug_profile build you can use: "
	@echo "            > make CONF=debug_profile"
	@echo "        To perform a release_profile build you can use: "
	@echo "            > make CONF=release_profile"
