cmake_minimum_required(VERSION 3.10.0)
project(RiDDLe VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

add_subdirectory(extern/json)

option(COMPUTE_NAMES "Compute RiDDLe names" OFF)

add_library(RiDDLe src/core.cpp src/scope.cpp src/env.cpp src/type.cpp src/constructor.cpp src/method.cpp src/item.cpp src/conjunction.cpp src/declaration.cpp src/statement.cpp src/expression.cpp src/compilation_unit.cpp src/lexer.cpp src/parser.cpp)
add_dependencies(RiDDLe json)
target_compile_features(RiDDLe PUBLIC cxx_std_17)
target_include_directories(RiDDLe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET utils)
    add_subdirectory(extern/utils)
    add_dependencies(RiDDLe utils)
endif()
target_link_libraries(RiDDLe PUBLIC json utils)

message(STATUS "Compute RiDDLe names: ${COMPUTE_NAMES}")
if(COMPUTE_NAMES)
    target_compile_definitions(RiDDLe PUBLIC COMPUTE_NAMES)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_options(RiDDLe PRIVATE /W4)
else()
    target_compile_options(RiDDLe PRIVATE -Wall -Wextra -Wpedantic -fsanitize=undefined)
    if (ENABLE_COVERAGE)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(RiDDLe PRIVATE --coverage)
            target_link_libraries(RiDDLe PUBLIC gcov)
        endif()
    endif()
endif()

set(CPACK_PROJECT_NAME RiDDLe)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
