#pragma once

namespace ast {

enum class AddressSpace {
  Undefined,
  Constant,
  Device,
  Thread,
  Shared,
};

} // namespace ast
