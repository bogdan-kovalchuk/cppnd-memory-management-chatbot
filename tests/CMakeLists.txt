cmake_minimum_required(VERSION 3.11.3)
project(ChatbotTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
if(MSVC)
    add_compile_options(/UNDEBUG)
else()
    add_compile_options(-UNDEBUG)
endif()

enable_testing()

add_executable(test_answer_graph_parser
    test_answer_graph_parser.cpp
    ../src/answer_graph_parser.cpp
)
target_include_directories(test_answer_graph_parser PRIVATE ${CMAKE_SOURCE_DIR}/../src)
add_test(NAME answer_graph_parser COMMAND test_answer_graph_parser)

add_executable(test_ownership_characterization
    test_ownership_characterization.cpp
    levenshtein.cpp
)
add_test(NAME ownership_characterization COMMAND test_ownership_characterization)

add_executable(test_ownership_edge_cases
    test_ownership_edge_cases.cpp
    levenshtein.cpp
)
add_test(NAME ownership_edge_cases COMMAND test_ownership_edge_cases)

add_executable(test_exclusive_handle
    test_exclusive_handle.cpp
)
target_include_directories(test_exclusive_handle PRIVATE ${CMAKE_SOURCE_DIR}/../src)
add_test(NAME exclusive_handle COMMAND test_exclusive_handle)

add_executable(test_chatbot_integration
    test_chatbot_integration.cpp
)
target_include_directories(test_chatbot_integration PRIVATE ${CMAKE_SOURCE_DIR}/../src)
add_test(NAME chatbot_integration COMMAND test_chatbot_integration)

add_executable(test_chatbot_copy_constructor
    test_chatbot_copy_constructor.cpp
    ../src/chatbot.cpp
    ../src/graphnode.cpp
    ../src/graphedge.cpp
)
target_include_directories(test_chatbot_copy_constructor PRIVATE
    ${CMAKE_SOURCE_DIR}/../src
    ${CMAKE_SOURCE_DIR}/support
)
add_test(NAME chatbot_copy_constructor COMMAND test_chatbot_copy_constructor)

add_executable(test_graphnode_move
    test_graphnode_move.cpp
)
add_test(NAME graphnode_move COMMAND test_graphnode_move)

add_executable(test_graphnode_move_edge_cases
    test_graphnode_move_edge_cases.cpp
)
add_test(NAME graphnode_move_edge_cases COMMAND test_graphnode_move_edge_cases)

add_executable(test_graphnode_move_comparison
    test_graphnode_move_comparison.cpp
)
add_test(NAME graphnode_move_comparison COMMAND test_graphnode_move_comparison)

add_executable(test_chatlogic_lifecycle
    test_chatlogic_lifecycle.cpp
)
add_test(NAME chatlogic_lifecycle COMMAND test_chatlogic_lifecycle)

add_executable(test_ownership_chain_integration
    test_ownership_chain_integration.cpp
)
add_test(NAME ownership_chain_integration COMMAND test_ownership_chain_integration)
