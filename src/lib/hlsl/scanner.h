#pragma once

#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "token.h"

namespace hlsl {

inline bool isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r';
}

inline bool isNumeric(char c) {
  return c >= '0' && c <= '9';
}

/// The scanner is responsible for taking a string of source code and breaking it into a list
/// of tokens.
class Scanner {
public:
  Scanner(const std::string_view& source, const std::string filename = "");

  /// Scan the source code and return a list of all tokens.
  std::vector<Token> scan();

  /// Scan the next token in the source code. This is used to stream tokens from the
  /// scanner rather than scanning the entire source code at once.
  Token scanNext();

  /// Return true if the scanner has reached the end of the source code.
  bool isAtEnd() const;

  const std::string_view& source() const { return _source; }

  const std::string& filename() const {
    return _filename;
  }

  int line() const {
    return _line;
  }

  int absoluteLine() const {
    return _absolteLine;
  }

private:
  bool scanToken();

  char advance();

  bool matchNext(char expected);

  char current() const;

  char peekAhead(int count = 1) const;

  void addToken(TokenType t);

  void scanPragma();

  void skipWS() {
    while (!isAtEnd() && isWhitespace(current())) {
      advance();
    }
  }

  const std::string_view _source;
  const size_t _size;
  std::list<Token> _tokens;
  size_t _start = 0;
  size_t _position = 0;
  size_t _lexemeLength = 0;
  std::string _filename;
  int _line = 1;
  int _absolteLine = 1;
};

} // namespace hlsl
