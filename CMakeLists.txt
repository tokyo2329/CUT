cmake_minimum_required(VERSION 3.20)

project(CUT)

option(TESTS "Use TESTS to enable testing" ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(COMPILE_FLAGS
  -Wall -Wextra
)

set(SRC
  src/buffer.c
  src/threads.c
  src/cpu_data.c
)

add_library(${PROJECT_NAME}-lib STATIC ${SRC})

target_compile_options(
  ${PROJECT_NAME}-lib
  PRIVATE
  ${COMPILE_FLAGS}
)

target_include_directories(
  ${PROJECT_NAME}-lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib)

if(TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()