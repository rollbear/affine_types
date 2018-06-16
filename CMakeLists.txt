cmake_minimum_required(VERSION 3.10)
project(affine_types)

set(CMAKE_CXX_STANDARD 17)
set(CXXFLAGS "${CXXFLAGS} -Wall -Wextra -pedantic -Werror")
add_executable(affine_types main.cpp include/affine_types.hpp)
target_include_directories(affine_types PRIVATE include)
