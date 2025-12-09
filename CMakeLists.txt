cmake_minimum_required(VERSION 3.16)
project(mpr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Shared include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add subprojects
add_subdirectory(server)
add_subdirectory(client)
