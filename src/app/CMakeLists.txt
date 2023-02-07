add_executable(hlslReflect ${LIB_SOURCE} main.cpp)
add_executable(hlslParse ${LIB_SOURCE} parse.cpp)

set_target_properties(hlslReflect PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)

set_target_properties(hlslParse PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)
