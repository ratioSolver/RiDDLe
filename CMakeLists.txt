cmake_minimum_required(VERSION 3.5.0)
project(RiDDLe VERSION 0.2.0 LANGUAGES CXX)

include(CTest)
enable_testing()

add_subdirectory(extern/utils)

add_library(RiDDLe src/scope.cpp src/env.cpp src/core.cpp src/type.cpp src/constructor.cpp src/method.cpp src/conjunction.cpp src/item.cpp src/lexer.cpp src/parser.cpp src/expression.cpp src/statement.cpp src/declaration.cpp src/compilation_unit.cpp)
target_link_libraries(RiDDLe PUBLIC utils)
target_include_directories(RiDDLe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
