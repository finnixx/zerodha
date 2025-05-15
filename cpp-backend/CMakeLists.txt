cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0167 NEW)

project(cpp_backend VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost 1.88 REQUIRED COMPONENTS atomic serialization system)
find_package(Threads REQUIRED)

add_executable(cpp_backend src/main.cpp)
target_include_directories(cpp_backend PRIVATE include)
target_link_libraries(cpp_backend PRIVATE Boost::atomic Boost::serialization Boost::system Threads::Threads)
