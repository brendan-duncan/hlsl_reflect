cmake_minimum_required(VERSION 3.22)

project(hlslReflect)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCE src/lib/ast.cpp
  src/lib/literal.cpp
  src/lib/match_literal.cpp
  src/lib/parser.cpp
  src/lib/scanner.cpp
  src/lib/token_type.cpp
  src/lib/token.cpp)

add_executable(hlslReflect ${LIB_SOURCE} src/app/main.cpp)
set_target_properties(hlslReflect PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)

add_executable(test  ${LIB_SOURCE}
  src/test/test_token_type.cpp
  src/test/test_scanner.cpp
  src/test/test.cpp
  src/test/main.cpp)
set_target_properties(test PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)
