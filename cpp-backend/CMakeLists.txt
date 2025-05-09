cmake_minimum_required(VERSION 3.10)
project(OrderBookProject)

set(CMAKE_CXX_STANDARD 17) # Or your preferred C++ standard

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

include_directories(include) # Tell the compiler where to find header files

add_executable(orderbook ${SOURCES})
# If you were using external libraries, you would link them here
# For example, if using nlohmann/json (assuming it's installed):
# find_package(nlohmann_json REQUIRED)
# target_link_libraries(orderbook nlohmann_json::nlohmann_json)
