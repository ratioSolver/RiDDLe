add_executable(riddle_lib_tests test_riddle.cpp)
target_link_libraries(riddle_lib_tests PRIVATE RiDDLe)

add_test(NAME RIDDLE_LibTest COMMAND riddle_lib_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})