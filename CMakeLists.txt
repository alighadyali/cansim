cmake_minimum_required(VERSION 3.25)

set(APP_NAME cansim)

project(${APP_NAME} VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND COMPILER_OPTS

    # common cxx options
    -Wall
    -Wextra
    -Wuninitialized
    -Wpedantic
    -Wunreachable-code
    $<$<CONFIG:RELEASE>:-Os>
    $<$<CONFIG:DEBUG>:-g>
    $<$<CONFIG:DEBUG>:-O0>
)

set(SRC_FILES
    src/main.cpp
)

add_executable(${APP_NAME} ${SRC_FILES})

target_compile_options(${APP_NAME} PRIVATE
    ${COMPILER_OPTS})