cmake_minimum_required(VERSION 3.9)
project(coroutine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-g -Wall")
file(GLOB SOURCE_FILE
  ${PROJECT_SOURCE_DIR}/*.cc
)
add_executable(${PROJECT_NAME} ${SOURCE_FILE})
