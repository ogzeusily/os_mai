cmake_minimum_required(VERSION 3.10)
project(OS_MAI_207_LAB_2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

set(SOURCES
    main.c
    linux/os_linux.c
)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/linux)

add_executable(dice_game ${SOURCES})
