cmake_minimum_required(VERSION 3.10)
project(affine_types)

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 17)
add_library(affine_types INTERFACE)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
        affine_types
        INTERFACE
        $<BUILD_INTERFACE:${INCLUDE_DIR}>
)
target_include_directories(
        affine_types
        INTERFACE
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
add_executable(self_test main.cpp)
target_link_libraries(self_test PRIVATE affine_types)

install(
        FILES
          include/affine_types.hpp
        DESTINATION
          ${CMAKE_INSTALL_INCLUDEDIR}
)