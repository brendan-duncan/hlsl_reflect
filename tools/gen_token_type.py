def enumName(s):
    if s == 'NULL': return 'Null'
    return s[0].upper() + s[1:]

fp = open('keywords.txt', 'rt')
keywords = [l.strip() for l in fp.readlines()]
fp.close()

fp = open('vector_matrix_types.txt', 'rt')
vectorMatrixTypes = [l.strip() for l in fp.readlines()]
fp.close()

fp = open('tokens.txt', 'rt')
tokens = [l.strip().split(' ') for l in fp.readlines()]
fp.close()

fp = open('types.txt', 'rt')
types = [l.strip() for l in fp.readlines()]
fp.close()

####################################################################################################
# reader/hlsl/token_type.h
####################################################################################################
fp = open('../src/lib/reader/hlsl/token_type.h', 'wt')

fp.write('''#pragma once
// Generated by tools/gen_token_type.py
#include <iostream>
#include <stdint.h>
#include <string>
#include <string_view>

namespace reader {
namespace hlsl {

enum class TokenType : uint32_t {
  Undefined,
  EndOfFile,
  IntLiteral,
  FloatLiteral,
  StringLiteral,
  Identifier,
''')

for k in tokens:
    fp.write('  {0},\n'.format(k[0]))

for k in keywords:
    fp.write('  {0},\n'.format(enumName(k)))

for k in vectorMatrixTypes:
    for i in range(1,5):
        fp.write('  {0}{1},\n'.format(enumName(k), i))
        for j in range(1,5):
            fp.write('  {0}{1}x{2},\n'.format(enumName(k), i, j))

fp.write('''};

inline std::ostream& operator<<(std::ostream& os, const TokenType& obj) {
  os << static_cast<std::underlying_type<TokenType>::type>(obj);
  return os;
}

const std::string& tokenTypeToString(TokenType t);

TokenType findTokenType(const std::string_view& s);

} // namespace hlsl
} // namespace reader
''')

fp.close()

####################################################################################################
# reader/hlsl/token_type.cpp
####################################################################################################
fp = open('../src/lib/reader/hlsl/token_type.cpp', 'wt')
fp.write('''// Generated by tools/gen_token_type.py
#include <array>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <variant>

#include "scanner/literal.h"
#include "scanner/template_types.h"
#include "token_type.h"

namespace reader {
namespace hlsl {

// Used for diagnostics
static const std::map<TokenType, std::string> _tokenTypeToString{
  {TokenType::Undefined, "Undefined"},
  {TokenType::EndOfFile, "EndOfFile"},
  {TokenType::IntLiteral, "IntLiteral"},
  {TokenType::FloatLiteral, "FloatLiteral"},
  {TokenType::StringLiteral, "StringLiteral"},
  {TokenType::Identifier, "Identifier"},
''')

tokenCount = 0
for k in tokens:
    tokenCount += 1
    fp.write('  {{TokenType::{0}, "{0}"}},\n'.format(k[0]))

for k in keywords:
    tokenCount += 1
    fp.write('  {{TokenType::{0}, "{0}"}},\n'.format(enumName(k)))

for k in vectorMatrixTypes:
    for i in range(1,5):
        tokenCount += 1
        fp.write('  {{TokenType::{0}{1}, "{0}{1}"}},\n'.format(enumName(k), i))
        for j in range(1,5):
            tokenCount += 1
            fp.write('  {{TokenType::{0}{1}x{2}, "{0}{1}x{2}"}},\n'.format(enumName(k), i, j))
fp.write('}; // _tokenTypeToString\n\n')

tokenStrings = {}

for k in tokens:
    tokenStrings[k[1]] = 'TokenType::' + k[0]

for k in keywords:
    tokenStrings[k] = 'TokenType::' + enumName(k)

for k in vectorMatrixTypes:
    for i in range(1,5):
        tokenCount += 1
        tokenStrings[k + str(i)] = 'TokenType::' + enumName(k) + str(i)
        for j in range(1,5):
            tokenCount += 1
            tokenStrings[k + str(i) + 'x' + str(j)] = 'TokenType::' + enumName(k) + str(i) + 'x' + str(j)

fp.write('''const std::string& tokenTypeToString(TokenType t) {
  auto ti = _tokenTypeToString.find(t);
  if (ti == _tokenTypeToString.end()) {
    static const std::string undefined{"Undefined"};
    return undefined;
  }
  return (*ti).second;
}

TokenType findTokenType(const std::string_view& lexeme) {
  size_t len = lexeme.length();
  size_t ci = 0;
''')

tokenNames = list(tokenStrings.keys())
tokenLengths = {}
tokenLengthCount = {}
for k in tokenNames:
    l = len(k)
    if l not in tokenLengths:
        tokenLengths[l] = {}
    charMap = tokenLengths[l]
    for ci in range(len(k)):
        c = k[ci]
        if ci == len(k) - 1:
            charMap[c] = tokenStrings[k]
            tokenLengthCount[l] = tokenLengthCount.get(l, 0) + 1
            break
        if c not in charMap:
            charMap[c] = {}
        charMap = charMap[c]

def _indent(fp, indent):
    fp.write(' ' * (indent * 2))

def writeCharMap(fp, charMap, indent, index):
    _indent(fp, indent)
    fp.write('switch (lexeme[{0}]) {{\n'.format(index))
    for c in charMap:
        _indent(fp, indent + 1)
        fp.write('case \'{0}\':\n'.format(c))
        if isinstance(charMap[c], str):
            _indent(fp, indent + 2)
            fp.write('return {0};\n'.format(charMap[c]))
        else:
            #fp.write('++ci;\n')
            writeCharMap(fp, charMap[c], indent + 2, index + 1)
            _indent(fp, indent + 1)
            fp.write('break;\n')
    _indent(fp, indent)
    fp.write('}\n')

lengths = list(tokenLengths.keys())
lengths.sort()
for l in lengths:
    fp.write('  if (len == {0}) {{\n'.format(l))
    charMap = tokenLengths[l]
    writeCharMap(fp, charMap, 2, 0)

    fp.write('  }\n')

fp.write('''
  return matchLiteral(lexeme);
}

} // namespace hlsl
} // namespace reader
''')

fp.close()

####################################################################################################
# reader/hlsl/token_to_ast.cpp
####################################################################################################
fp = open('../src/lib/reader/hlsl/token_to_ast.cpp', 'wt')
fp.write('''#pragma once
// Generated by tools/gen_token_type.py
#include "token_to_ast.h"

#include <map>

namespace reader {
namespace hlsl {

static const std::map<TokenType, ast::BaseType> _tokenTypeToBaseType{
''')

for k in types:
  fp.write('  {{TokenType::{0}, ast::BaseType::{0}}},\n'.format(enumName(k)))

for k in vectorMatrixTypes:
    for i in range(1,5):
        fp.write('  {{TokenType::{0}{1}, ast::BaseType::{0}{1}}},\n'.format(enumName(k), i))
        for j in range(1,5):
            fp.write('  {{TokenType::{0}{1}x{2}, ast::BaseType::{0}{1}x{2}}},\n'.format(enumName(k), i, j))

fp.write('''};

ast::BaseType tokenTypeToBaseType(TokenType t) {
  auto ti = _tokenTypeToBaseType.find(t);
  if (ti == _tokenTypeToBaseType.end()) {
    return ast::BaseType::Undefined;
  }
  return (*ti).second;
}

} // namespace hlsl
} // namespace reader
''')

####################################################################################################
# ast/base_type.h
####################################################################################################
fp = open('../src/lib/ast/base_type.h', 'wt')
fp.write('''#pragma once
// Generated by tools/gen_token_type.py

#include <string_view>

namespace ast {

enum class BaseType {
''')

for k in types:
  fp.write("  {0},\n".format(enumName(k)))

for k in vectorMatrixTypes:
    for i in range(1,5):
        fp.write('  {0}{1},\n'.format(enumName(k), i))
        for j in range(1,5):
            fp.write('  {0}{1}x{2},\n'.format(enumName(k), i, j))

fp.write('''};

const std::string_view& baseTypeToString(BaseType t);

inline bool isSamplerBaseType(BaseType t) {
  return t == BaseType::Sampler ||
      t == BaseType::SamplerState ||
      t == BaseType::SamplerComparisonState;
}

} // namespace ast
''')
fp.close()


####################################################################################################
# ast/base_type.cpp
####################################################################################################
fp = open('../src/lib/ast/base_type.cpp', 'wt')
fp.write('''// Generated by tools/gen_token_type.py
#include "base_type.h"

#include <map>

namespace ast {

static const std::map<BaseType, std::string_view> _baseTypeToString{
''')

for k in types:
  fp.write('''  {{BaseType::{0}, "{1}"}},\n'''.format(enumName(k), k))

for k in vectorMatrixTypes:
    for i in range(1,5):
        fp.write('''  {{BaseType::{0}{1}, "{2}{1}"}},\n'''.format(enumName(k), i, k))
        for j in range(1,5):
            fp.write('''  {{BaseType::{0}{1}x{2}, "{3}{1}x{2}"}},\n'''.format(enumName(k), i, j, k))

fp.write('''};

const std::string_view& baseTypeToString(BaseType t) {
  auto ti = _baseTypeToString.find(t);
  if (ti == _baseTypeToString.end()) {
    static const std::string_view undefined{"Undefined"};
    return undefined;
  }
  return (*ti).second;
}

} // namespace ast
''')
fp.close()
