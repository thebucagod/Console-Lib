cmake_minimum_required(VERSION 3.15)

project(ConsoleLib LANGUAGES CXX)

add_library(console src/console.cpp)
target_include_directories(console PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(console PRIVATE cxx_std_17)

option(BUILD_EXAMPLES "Собирать ли примеры использования?" ON)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()