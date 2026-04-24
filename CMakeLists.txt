cmake_minimum_required(VERSION 3.16)
project(todo_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

add_executable(todo_app main.cc)

target_link_libraries(todo_app PRIVATE Drogon::Drogon)
