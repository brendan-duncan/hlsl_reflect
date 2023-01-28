#include "scanner.h"

namespace hlsl {

Scanner::Scanner(const char* source, const char* filename)
    : _source(source)
    , _filename(filename)
    , _line(1)
    , _token(TokenType::EndOfFile, "", 0)
    , _float(0.0f)
    , _int(0)
    , _identifier(nullptr)
    , _position(0) {
}   

Token Scanner::Next() {

}


} // namespace hlsl
