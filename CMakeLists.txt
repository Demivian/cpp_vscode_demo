cmake_minimum_required(VERSION 3.24)
project(MyProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)

function(enable_project_warnings target)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${target} PRIVATE /utf-8 /W4 /permissive- /Zc:__cplusplus)
    target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -fdiagnostics-color=always)
  endif()
endfunction()

add_subdirectory(src)
add_subdirectory(app)
enable_testing()
add_subdirectory(tests)

# Python bindings can be added later as a separate target if needed.
