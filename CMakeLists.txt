cmake_minimum_required(VERSION 3.31)
project(SqLite)

set(CMAKE_CXX_STANDARD 20)

# Include the current directory (so sqlite3.h can be found)
include_directories(${CMAKE_SOURCE_DIR})

add_executable(SqLite
        library_management.cpp
        sqlite3.c    # sqlite3.c is already in your project folder
)
