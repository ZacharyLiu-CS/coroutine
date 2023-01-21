cmake_minimum_required(VERSION 3.9)
project(coroutine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCE_FILE
  ${PROJECT_SOURCE_DIR}/*.cc
)
add_executable(${PROJECT_NAME} ${SOURCE_FILE})
