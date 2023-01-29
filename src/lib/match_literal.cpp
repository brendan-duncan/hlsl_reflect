#include "match_literal.h"

namespace hlsl {

static bool isNumeric(char c) {
  return c >= '0' && c <= '9';
}

static bool isHexAlpha(char c) {
  return c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F';
}

static bool isAlpha(char c) {
  return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
}

static bool isAlphaNumeric(char c) {
  return isAlpha(c) || isNumeric(c);
}

TokenType matchLiteral(const std::string_view& lexeme) {
  char c = lexeme[0];
  const size_t len = lexeme.length();
  // Using std::regex instead of manually parsing the string was 12x slower.
  // /-?0x[0-9a-fA-F]+[uUL]?|-?0[0-7]*[uUL]?|-?[1-9][0-9]*[uUL]?/
  // /-?[0-9]+(.[0-9]*)?([eE][+-]?[0-9]+)?[FfHh]?/
  if (c == '-' || c == '.' || isNumeric(c)) {
    size_t ci = 0;
    bool hasDecimal = c == '.';
    bool hasExponent = false;
    bool isHex = false;
    bool isOctal = false;
    bool isFloat = hasDecimal;

    if (c == '-') {
      if (len == 1) {
        return TokenType::Undefined;
      }
      ci++;
      c = lexeme[ci];
    }

    if (c == '0') {
      // Octal or hex
      if ((ci + 1) < len && lexeme[ci + 1] == 'x') {
        isHex = true;
        ci += 2;
      } else {
        isOctal = true;
        ci++;
      }
    }

    if (ci >= len) {
      if (isHex) {
        return TokenType::Undefined;
      }
      return isFloat ? TokenType::FloatLiteral : TokenType::IntLiteral;
    }

    c = lexeme[ci];
    while (ci < len && (isNumeric(c) || isHexAlpha(c))) {
      if (isOctal && (isHexAlpha(c) || c > '7')) {
        return TokenType::Undefined;
      }
      if (!isHex && isHexAlpha(c)) {
        return TokenType::Undefined;
      }
      ci++;
      if (ci == len) {
        return isFloat ? TokenType::FloatLiteral : TokenType::IntLiteral;
      }
      c = lexeme[ci];
    }

    if (c == 'u' || c == 'U' || c == 'L') {
      ci++;
      if (ci == len) {
        return TokenType::IntLiteral;
      }
      return TokenType::Undefined;
    }

    if (c == '.') {
      if (hasDecimal || isHex) {
        return TokenType::Undefined;
      }
      hasDecimal = true;
      isOctal = false;
      isFloat = true;
      ci++;
      if (ci == len) {
        return TokenType::FloatLiteral;
      }

      c = lexeme[ci];
      while (ci < len && isNumeric(c)) {
        ci++;
        if (ci == len) {
          return TokenType::FloatLiteral;
        }
        c = lexeme[ci];
      }
    }

    if (c == 'e' || c == 'E') {
      ci++;
      if (ci == len) {
        return TokenType::Undefined;
      }
      c = lexeme[ci];
      if (c == '+' || c == '-') {
        ci++;
        if (ci == len) {
          return TokenType::Undefined;
        }
        c = lexeme[ci];
      }
      while (ci < len && isNumeric(c)) {
        ci++;
        if (ci == len) {
          return TokenType::FloatLiteral;
        }
        c = lexeme[ci];
      }
      if (ci == len) {
        return TokenType::FloatLiteral;
      }
    }

    if (c == 'f' || c == 'F' || c == 'h' || c == 'H') {
      ci++;
      return ci != len ? TokenType::Undefined
        : isHex ? TokenType::Undefined : TokenType::FloatLiteral;
    }
    if (c == 'L') {
      ci++;
      return ci != len ? TokenType::Undefined
          : isFloat ? TokenType::FloatLiteral : TokenType::IntLiteral;
    }
    if (c == 'u' || c == 'U') {
      ci++;
      return ci != len ? TokenType::Undefined
          : isFloat ? TokenType::Undefined : TokenType::IntLiteral;
    }

    return TokenType::Undefined;
  }

  if (isAlpha(c)) {
    bool isValid = true;
    size_t ci = 1;
    
    for (size_t ci = 1; ci < len; ++ci) {
      c = lexeme[ci];
      if (!(isAlphaNumeric(c))) {
        isValid = false;
        break;
      }
    }
    if (isValid) {
      return TokenType::Identifier;
    }
  }

  return TokenType::Undefined;
}

} // namespace hlsl
