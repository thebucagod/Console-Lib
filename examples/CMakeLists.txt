add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example PRIVATE console)
target_compile_options(basic_example PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)