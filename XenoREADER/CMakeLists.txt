cmake_minimum_required(VERSION 3.30)

project(XenoREADER)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_C_COMPILER_ID EQUAL "GNU" OR CMAKE_C_COMPILER_ID EQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -O3")
elseif(CMAKE_C_COMPILER_ID EQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX /O3")
endif()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
              source/main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE XenoReader)