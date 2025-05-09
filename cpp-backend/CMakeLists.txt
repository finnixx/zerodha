cmake_minimum_required(VERSION 3.22)  # Ensure we're using at least CMake 3.22

# Project name and version
project(cpp_backend VERSION 1.0)

# Set C++ standard to 20 (ensure modern C++ features are available)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.88 REQUIRED COMPONENTS atomic)

include_directories(include)

# Add the source files to the project
add_executable(cpp_backend src/main.cpp)
target_include_directories(cpp_backend PRIVATE include)

# Link only the required Boost components (atomic in this case)
target_link_libraries(cpp_backend Boost::atomic)

# # Ensure we use pthread support on Linux/macOS (for thread support)
# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
#     target_compile_options(cpp_backend PRIVATE -std=c++20 -pthread)
# elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
#     target_compile_options(cpp_backend PRIVATE /std:c++20)
# endif()

# # Optionally, set output directory for executables
# set_target_properties(cpp_backend PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

