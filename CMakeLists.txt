cmake_minimum_required(VERSION 3.5.0)
project(RiDDLe VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")

include(CTest)
enable_testing()

add_subdirectory(extern/utils)

option(ENABLE_VISUALIZATION "Enable visualization features" OFF)

add_library(RiDDLe src/scope.cpp src/env.cpp src/core.cpp src/type.cpp src/constructor.cpp src/method.cpp src/conjunction.cpp src/item.cpp src/lexer.cpp src/parser.cpp src/expression.cpp src/statement.cpp src/declaration.cpp src/compilation_unit.cpp)
add_dependencies(RiDDLe utils)
target_link_libraries(RiDDLe PUBLIC utils)
target_include_directories(RiDDLe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

message(STATUS "Enable visualization features: ${ENABLE_VISUALIZATION}")

if(ENABLE_VISUALIZATION)
    target_compile_definitions(RiDDLe PUBLIC ENABLE_VISUALIZATION)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    if (ENABLE_COVERAGE)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
            target_link_libraries(${PROJECT_NAME} PUBLIC gcov)
        endif()
    endif()
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
