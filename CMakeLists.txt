cmake_minimum_required(VERSION 3.15)

project(ConsoleLib LANGUAGES CXX)

if(NOT WIN32)
	message(FATAL_ERROR "console supports Windows only")
endif()

add_library(console src/console.cpp)
target_include_directories(console PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(console PRIVATE cxx_std_17)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(_console_top_level ON)
else()
	set(_console_top_level OFF)
endif()

option(BUILD_EXAMPLES "Build usage examples" ${_console_top_level})