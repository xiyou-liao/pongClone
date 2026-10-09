cmake_minimum_required(VERSION 4.0)
project(pongClone)

set(CMAKE_CXX_STANDARD 23)

add_executable(pongClone main.cpp)

target_include_directories(pongClone PRIVATE
        C:/raylib/include
)

target_link_directories(pongClone PRIVATE
        C:/raylib/lib
)

target_link_libraries(pongClone PRIVATE
        raylib
        opengl32
        gdi32
        winmm
)