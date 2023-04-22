set(LIB_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/lib/ast/ast_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ast/ast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ast/base_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ast/operator.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/parser/effect_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/scanner/literal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/token_to_ast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/token_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/reader/hlsl/token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/visitor/visitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/visitor/prune_tree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/util/string_util.cpp)

add_compile_definitions(DATA_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/_data)

add_subdirectory(app)
add_subdirectory(test)
