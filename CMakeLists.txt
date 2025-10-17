cmake_minimum_required(VERSION 3.30)

project(xenoreader)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_options(${PROJECT_NAME} PRIVATE -O2 -Werror)
target_sources(${PROJECT_NAME} PRIVATE
              source/main.c
              source/XenoReader.c)
target_link_options(${PROJECT_NAME} PRIVATE -s)