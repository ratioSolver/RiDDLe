add_executable(lexer_lib_tests test_lexer.cpp)
target_link_libraries(lexer_lib_tests PRIVATE RiDDLe SeMiTONE json)

add_executable(parser_lib_tests test_parser.cpp)
target_link_libraries(parser_lib_tests PRIVATE RiDDLe SeMiTONE json)

add_test(NAME RIDDLE_LexerTest COMMAND lexer_lib_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME RIDDLE_ParserTest COMMAND parser_lib_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})