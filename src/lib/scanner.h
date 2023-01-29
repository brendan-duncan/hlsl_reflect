#pragma once

#include <list>
#include <string>
#include <string_view>
#include <vector>

#include "token.h"

namespace hlsl {

class Scanner {
public:
    Scanner(const std::string_view& source);

    std::vector<Token> scan();

    Token scanNext();

    bool isAtEnd() const;

private:
  bool scanToken();

  char advance();

  bool matchNext(char expected);

  char current() const;

  char peekAhead(int count = 1) const;

  void addToken(TokenType t);

  void skipPragma();

  const std::string _source;
  const size_t _size;
  std::list<Token> _tokens;
  size_t _start = 0;
  size_t _position = 0;
  size_t _lexemeLength = 0;
  int _line = 1;
};

} // namespace hlsl
