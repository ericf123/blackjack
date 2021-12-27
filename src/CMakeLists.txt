add_subdirectory(lib)
add_subdirectory(cli)
add_subdirectory(tui)

add_executable(testcard test.cpp)
target_include_directories(testcard PUBLIC lib/include)