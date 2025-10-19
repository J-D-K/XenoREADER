cmake_minimum_required(VERSION 3.30)

project(XenoREADER)

# This is used by both projects.
include_directories(libXenoReader/include)

add_subdirectory(libXenoReader)
add_subdirectory(XenoREADER)