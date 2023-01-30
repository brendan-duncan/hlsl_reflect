add_executable(hlslReflect ${LIB_SOURCE} main.cpp)

set_target_properties(hlslReflect PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)
