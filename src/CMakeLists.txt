set(LIB_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/ast/ast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/ast/ast_node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/ast/data_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/visitor/visitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/literal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/scanner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/token_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hlsl/token.cpp)

add_compile_definitions(DATA_PATH=${CMAKE_CURRENT_SOURCE_DIR}/test/_data)

add_subdirectory(app)
add_subdirectory(test)
