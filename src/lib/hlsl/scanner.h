#pragma once

#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "token.h"

namespace hlsl {

/// The scanner is responsible for taking a string of source code and breaking it into a list
/// of tokens.
class Scanner {
public:
  Scanner(const std::string_view& source);

  /// Scan the source code and return a list of all tokens.
  std::vector<Token> scan();

  /// Scan the next token in the source code. This is used to stream tokens from the
  /// scanner rather than scanning the entire source code at once.
  Token scanNext();

  /// Return true if the scanner has reached the end of the source code.
  bool isAtEnd() const;

private:
  bool scanToken();

  char advance();

  bool matchNext(char expected);

  char current() const;

  char peekAhead(int count = 1) const;

  void addToken(TokenType t);

  void skipPragma();

  const std::string_view _source;
  const size_t _size;
  std::list<Token> _tokens;
  size_t _start = 0;
  size_t _position = 0;
  size_t _lexemeLength = 0;
  int _line = 1;
};

} // namespace hlsl
