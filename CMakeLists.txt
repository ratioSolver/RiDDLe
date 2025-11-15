cmake_minimum_required(VERSION 3.21)
project(RiDDLe VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

option(COMPUTE_NAMES "Compute RiDDLe names" OFF)

add_library(RiDDLe src/core.cpp src/scope.cpp src/env.cpp src/type.cpp src/items.cpp src/types.cpp src/constructor.cpp src/method.cpp src/term.cpp src/conjunction.cpp src/declaration.cpp src/statement.cpp src/expression.cpp src/compilation_unit.cpp src/lexer.cpp src/parser.cpp)
target_compile_features(RiDDLe PUBLIC cxx_std_17)
target_include_directories(RiDDLe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
if(NOT TARGET json)
    add_subdirectory(extern/json)
endif()
add_dependencies(RiDDLe json)
target_link_libraries(RiDDLe PUBLIC json)
setup_sanitizers(RiDDLe)

message(STATUS "Compute RiDDLe names: ${COMPUTE_NAMES}")
if(COMPUTE_NAMES)
    target_compile_definitions(RiDDLe PUBLIC COMPUTE_NAMES)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME RiDDLe)
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
