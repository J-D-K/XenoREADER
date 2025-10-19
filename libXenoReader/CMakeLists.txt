cmake_minimum_required(VERSION 3.30)

project(XenoReader)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# This needs to be different according to which compiler.
# I know there's a newer way to do this, but I don't care.
if(CMAKE_C_COMPILER_ID EQUAL "GNU" OR CMAKE_C_COMPILER_ID EQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -O3")
elseif(CMAKE_C_COMPILER_ID EQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /O3")
endif()

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_sources(${PROJECT_NAME} PRIVATE
              source/DynamicArray.c
              source/XenoBuffer.c
              source/XenoDir.c
              source/XenoFile.c
              source/XenoReader.c)
