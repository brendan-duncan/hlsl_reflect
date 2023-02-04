#pragma once

#include "../token_type.h"

namespace hlsl {
  
enum class Operator {
  Undefined,

  Equal,
  SubtractEqual,
  MultiplyEqual,
  DivideEqual,
  ModuloEqual,
  LeftShiftEqual,
  RightShiftEqual,
  AndEqual,
  OrEqual,
  XorEqual,

  PlusPlus,
  MinusMinus,

  OrOr,
  AndAnd,
  Or,
  Xor,
  And,
  EqualEqual,
  NotEqual,
  Less,
  LessEqual,
  Greater,
  GreaterEqual,
  LeftShift,
  RightShift,
  Add,
  Subtract,
  Multiply,
  Divide,
  Modulo,
  Not,
  AddEqual,
  BitNot,
};

const std::string_view& operatorToString(Operator op);

inline Operator tokenTypeToAssignmentOperatator(TokenType t) {
  switch (t) {
    case TokenType::Equal: return Operator::Equal;
    case TokenType::MinusEqual: return Operator::SubtractEqual;
    case TokenType::PlusEqual: return Operator::AddEqual;
    case TokenType::StarEqual: return Operator::MultiplyEqual;
    case TokenType::SlashEqual: return Operator::DivideEqual;
    case TokenType::PercentEqual: return Operator::ModuloEqual;
    case TokenType::LessLessEqual: return Operator::LeftShiftEqual;
    case TokenType::GreaterGreaterEqual: return Operator::RightShiftEqual;
    case TokenType::AmpersandEqual: return Operator::AndEqual;
    case TokenType::PipeEqual: return Operator::OrEqual;
    case TokenType::CaretEqual: return Operator::XorEqual;
    default: return Operator::Undefined;
  }
}

inline bool isAssignmentOperator(TokenType t) {
  return tokenTypeToAssignmentOperatator(t) != Operator::Undefined;
}

} // namespace hlsl
