cmake_minimum_required(VERSION 3.21)
project(untitled28)

set(CMAKE_CXX_STANDARD 14)

add_executable(untitled28 main.cpp)
target_link_libraries(${PROJECT_NAME} -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer)
