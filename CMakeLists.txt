cmake_minimum_required(
    VERSION 3.20.0
)

project(waiw)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/interpreter.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})