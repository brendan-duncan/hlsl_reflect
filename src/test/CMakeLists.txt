add_executable(test  ${LIB_SOURCE}
  hlsl/test_token_type.cpp
  hlsl/test_parser.cpp
  hlsl/test_scanner.cpp
  test.cpp
  main.cpp)

set_target_properties(test PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)
