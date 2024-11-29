cmake_minimum_required(VERSION 3.10)

# Project name and version
project(TerseDecompress VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_OSX_ARCHITECTURES "x86_64")

# Add the include directory
# include_directories(include)

# Gather all source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(TerseDecompress ${SOURCES})
