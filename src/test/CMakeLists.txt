add_executable(test  ${LIB_SOURCE}
  test.cpp
  main.cpp)

set_target_properties(test PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)
