cmake_minimum_required(VERSION 3.14)
project(OrderBookProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use folders to organize in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include header files
include_directories(include)
find_package(Boost 1.70 REQUIRED)
# Source files
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# Executable
add_executable(order_book ${SOURCES})

# For multithreading support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(order_book PRIVATE Threads::Threads)

# Treat warnings as errors (optional)
# target_compile_options(order_book PRIVATE -Wall -Wextra -Werror)

