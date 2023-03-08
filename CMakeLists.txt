cmake_minimum_required(VERSION 3.0.0)
project(RiDDLe VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(GenerateExportHeader)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(CTest)
enable_testing()

option(COMPUTE_NAMES "Computes the objects' names" OFF)
option(BUILD_LISTENERS "Builds the core's listeners" OFF)

add_subdirectory(extern/utils)

file(GLOB RIDDLE_SOURCES src/*.cpp)
file(GLOB RIDDLE_HEADERS include/*.h)

add_library(${PROJECT_NAME} SHARED ${RIDDLE_SOURCES})
add_dependencies(${PROJECT_NAME} utils)
GENERATE_EXPORT_HEADER(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<INSTALL_INTERFACE:>)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)

message(STATUS "Compute names:          ${COMPUTE_NAMES}")
if(COMPUTE_NAMES)
    target_compile_definitions(${PROJECT_NAME} PUBLIC COMPUTE_NAMES)
endif()

message(STATUS "Build core listeners:   ${BUILD_LISTENERS}")
if(BUILD_LISTENERS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC BUILD_LISTENERS)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    if (ADD_COVERAGE)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
            target_link_libraries(${PROJECT_NAME} PUBLIC gcov)
        endif()
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    FILES ${RIDDLE_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/riddle_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)
configure_package_config_file(src/${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME} PATH_VARS CMAKE_INSTALL_INCLUDEDIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake VERSION 1.0.0 COMPATIBILITY SameMajorVersion)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)