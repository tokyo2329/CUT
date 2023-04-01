cmake_minimum_required(VERSION 3.20)

project(CUT)

set(CMAKE_C_STANDARD 11)
set(COMPILE_FLAGS 
  -Wall -Wextra
)

set(SRC 
  main.c
)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  ${COMPILE_FLAGS}
)

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)