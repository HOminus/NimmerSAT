cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(NimmerSAT)

function (add_compiler_flag flag)
    target_compile_options(NimmerSAT PRIVATE ${flag})
endfunction()

function (add_linker_flag flag)
    target_link_options(NimmerSAT PRIVATE ${flag})
endfunction()

add_executable(NimmerSAT src/main.cpp
                        src/Formula/Cache.cpp
                        src/Solver/Assign.cpp
                        src/Dimacs/Dimacs.cpp)

target_include_directories(NimmerSAT PRIVATE src)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(NimmerSAT PRIVATE -Wall -Wextra -Wpedantic)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compiler_flag(-DNIMMERSAT_DEBUG)
    add_compiler_flag(-D_GLIBCXX_DEBUG)
    add_compiler_flag(-g3)
    add_compiler_flag(-rdynamic)
#    add_compiler_flag(-fsanitize=address)
#    add_linker_flag(-fsanitize=address)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release") # And linux
    add_compiler_flag(-O3)
    add_compiler_flag(-march=native)
endif ()
