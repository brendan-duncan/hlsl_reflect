#include "scanner.h"

#include <iterator>
#include <map>
#include <string_view>

#include "scanner/literal.h"
#include "scanner/template_types.h"
#include "token_type.h"

namespace hlsl {

Scanner::Scanner(const std::string_view& source, const std::string filename)
    : _source(source)
    , _size(source.size())
    , _filename(filename) {}

std::vector<Token> Scanner::scan() {
  while (!isAtEnd()) {
    _start = _position;
    _lexemeLength = 0;
    if (!scanToken()) {
      break;
    }
  }
  _tokens.emplace_back(TokenType::EndOfFile, "");
  std::vector<Token> tokens;
  std::move(_tokens.begin(), _tokens.end(), std::back_inserter(tokens));
  return tokens;
}

Token Scanner::scanNext() {
  if (!_tokens.empty()) {
    Token tk = _tokens.front();
    _tokens.pop_front();
    return tk;
  }

  while (!isAtEnd()) {
    _start = _position;
    _lexemeLength = 0;
    if (!scanToken()) {
      break;
    }
    if (!_tokens.empty()) {
       Token tk = _tokens.front();
      _tokens.pop_front();
      return tk;
    }
  }

  return Token{TokenType::EndOfFile, ""};
}

bool Scanner::isAtEnd() const { return _position >= _size; }

char Scanner::advance() { 
  char c = current();
  ++_position;
  ++_lexemeLength;
  return c;
}

bool Scanner::matchNext(char expected) {
  bool nextMatches = (current() == expected);
  if (nextMatches) {
    advance();
  }
  return nextMatches;
}

char Scanner::current() const {
  if (_position >= _size) {
    return '\0';
  }
  return _source[_position];
}

char Scanner::peekAhead(int count) const {
  if ((_position + count) >= _size) {
    return '\0';
  }
  return _source[_position + count];
}

void Scanner::addToken(TokenType t) {
  std::string_view lexeme = _source.substr(_start, _position - _start);
  _tokens.emplace_back(t, lexeme);
}

void Scanner::scanPragma() {
  skipWS();

  // Parse line pragma if present, e.g. #line 1 "file.hlsl"
  // Otherwise, just skip the rest of the line.
  static const char *linePragma = "line";
  int ci = 0;
  bool isLine = true;
  while (!isAtEnd() && current() != '\n') {
    if (ci < 4) {
      isLine &= current() == linePragma[ci++];
    }
    if (ci == 4 && isLine) {
      ci++;
      advance();
      skipWS();
      if (isNumeric(current())) {
        _line = 0;
        while (!isAtEnd() && isNumeric(current())) {
          _line = _line * 10 + (current() - '0');
          advance();
        }
      }
      while (!isAtEnd() && isWhitespace(current())) {
        advance();
      }
      if (current() == '"') {
        advance();
        int start = _position;
        int end = _position;
        while (!isAtEnd() && current() != '\n' && current() != '"') {
          end++;
          advance();
        }
        if (current() == '"') {
          advance();
        }
        _filename = _source.substr(start, end - start);
      }
      continue;
    }
    advance();
  }
  advance();
  
  _line++;
  _absoluteLine++;
}

bool Scanner::scanToken() {
  // Find the longest consecutive set of characters that match a rule.
  // This string of consecutive characters is the lexeme.
  char c = advance();

  // Skip whitespace
  if (isWhitespace(c)) {
    return true;
  }

  // Skip line-feed, adding to the line counter.
  if (c == '\n') {
    _line++;
    _absoluteLine++;
    return true;
  }
 
  if (c == '#') {
    scanPragma();
    return true;
  }

  if (c == '/') {
    char next = current();
    // If it's a // comment, skip everything until the next line-feed.
    if (next == '/') {
      while (c != '\n') {
        if (isAtEnd()) {
          return true;
        }
        c = advance();
      }
      // skip the linefeed
      _line++;
      _absoluteLine++;
      return true;
    } else if (next == '*') {
      // If it's a / * block comment, skip everything until the matching * /,
      // allowing for nested block comments.
      advance();
      int commentLevel = 1;
      while (commentLevel > 0) {
        if (isAtEnd()) {
          return true;
        }
        c = advance();
        if (c == '\n') {
          _line++;
          _absoluteLine++;
        } else if (c == '*') {
          next = current();
          if (next == '/') {
            advance();
            --commentLevel;
            if (commentLevel == 0) {
              return true;
            }
          }
        } else if (c == '/') {
          next = current();
          if (next == '*') {
            advance();
            commentLevel++;
          }
        }
      }
      return true;
    }
  }

  if (c == '"') {
    c = advance();
    // If it's a string, scan until the next " or end of file.
    while (c != '"') {
      if (isAtEnd()) {
        return true;
      }
      c = advance();
      if (c == '\\') {
        // Skip the next character, which is escaped.
        advance();
      }
    }
    addToken(TokenType::StringLiteral);
    return true;
  }

  TokenType matchType = TokenType::Undefined;

  std::string_view src{_source};

  while (true) {
    std::string_view lexeme = src.substr(_start, _lexemeLength);
    TokenType matchedType = findTokenType(lexeme);

    // The exception to "longest lexeme" rule is '>>'. In the case of 1>>2, it's a
    // shift_right.
    // In the case of array<vec4<f32>>, it's two greater_than's (one to close the vec4,
    // and one to close the array).
    // I don't know of a great way to resolve this, so '>>' is special-cased and if
    // there was a less_than up to some number of tokens previously, and the token prior to
    // that is a keyword that requires a '<', then it will be split into two greater_than's;
    // otherwise it's a shift_right.
    if (c == '>' && peekAhead() == '>') {
      bool foundLessThan = false;
      int count = 0;
      for (auto ti = _tokens.rbegin(); ti != _tokens.rend(); ++ti, ++count) {
        if (count >= 4) {
          break;
        }
        if ((*ti).type() == TokenType::Less) {
          auto pti = ti;
          ++pti;
          if (pti != _tokens.rend()) {
            if (isTemplateType((*pti).type())) {
              foundLessThan = true;
            }
            break;
          }
        }
      }

      // If there was a Less in the recent token history, then this is probably a
      // Greater.
      if (foundLessThan) {
        addToken(matchedType);
        return true;
      }
    }

    // The current lexeme may not match any rule, but some token types may be invalid for
    // part of the string but valid after a few more characters.
    // For example, 0x.5 is a hex_float_literal. But as it's being scanned,
    // "0" is a int_literal, then "0x" is invalid. If we stopped there, it would return
    // the int_literal "0", but that's incorrect. So if we look forward a few characters,
    // we'd get "0x.", which is still invalid, followed by "0x.5" which is the correct
    // hex_float_literal. So that means if we hit an non-matching string, we should look
    // ahead up to two characters to see if the string starts matching a valid rule again.
    if (matchedType == TokenType::Undefined) {
      std::string_view lookAheadLexeme = lexeme;
      int lookAhead = 0;
      const int maxLookAhead = 2;
      for (int li = 0; li < maxLookAhead; ++li) {
        lookAheadLexeme = src.substr(_start, _lexemeLength + 1 + li);
        matchedType = findTokenType(lookAheadLexeme);
        if (matchedType != TokenType::Undefined) {
          lookAhead = li;
          break;
        }
      }

      if (matchedType == TokenType::Undefined) {
        if (matchType == TokenType::Undefined) {
          return false;
        }
        _position--;
        addToken(matchType);
        return true;
      }

      lexeme = lookAheadLexeme;
      _position += lookAhead + 1;
      _lexemeLength += lookAhead + 1;
    }

    matchType = matchedType;

    if (isAtEnd()) {
      break;
    }

    advance();
  }

  // We got to the end of the input stream. Then the token we've ready so far is it.
  if (matchType == TokenType::Undefined) {
    return false;
  }

  addToken(matchType);
  
  return true;
}

}  // namespace hlsl
