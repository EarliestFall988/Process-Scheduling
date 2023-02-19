cmake_minimum_required(VERSION 2.8)

# ALL OF THE SPECIFIED NAMES IN THIS CMAKE ARE REQUIRED.
# Set the project name variable to assignment3.
# THIS IS REQUIRED
project(hw2)

# Add your CMAKE_C_FLAGS AND CMAKE_CXX_FLAGS here
# Make SURE to use -std=c11 and -std=c++11 where relevant.
# The options specified below are required, but additional options
#  may be used.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra -Wshadow -Werror")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wshadow -Werror")

# Add our include directory to CMake's search paths.
# THIS IS REQUIRED
include_directories(include)

# Create library from dyn_array so we can use it later.
add_library(dyn_array SHARED src/dyn_array.c)

# Compile the analysis executable.
add_executable(analysis src/analysis.c)

# link the dyn_array library we compiled against our analysis executable.
target_link_libraries(analysis dyn_array)

# Compile the the tester executable.
add_executable(hw2_test test/tests.cpp)

target_compile_definitions(hw2_test PRIVATE)

# Link ${PROJECT_NAME}_test with dyn_array and gtest and pthread libraries
target_link_libraries(hw2_test gtest pthread dyn_array)

enable_testing()
add_test(NAME hw2_test
    COMMAND hw2_test)
