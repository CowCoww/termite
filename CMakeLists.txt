cmake_minimum_required(VERSION 3.15)
project(termite VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(termite STATIC
        src/terminal.cpp
        src/input.cpp
        src/canvas.cpp
        src/app.cpp
        src/clock.cpp
)

# Public include path
target_include_directories(termite PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(termite PUBLIC cxx_std_20)