cmake_minimum_required(VERSION 3.1)
project(Blacjack VERSION 0.1
                 LANGUAGES CXX)

find_package(Clang REQUIRED)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(CMAKE_BUILD_TYPE debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)