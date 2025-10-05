cmake_minimum_required(VERSION 3.10)
project(OS_MAI_207_LAB_1 C)

# Устанавливаем стандарт C
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Компилируем три отдельные программы
add_executable(parent parent.c os_linux.c)
add_executable(child1 child1.c)
add_executable(child2 child2.c)