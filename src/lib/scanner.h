#include "token.h"

namespace hlsl {

class Scanner {
public:
    Scanner(const char* source, const char* filename);

    Token Next();

    Token GetToken() const { return _token;}

    float GetFloat() const { return _float; }

    int GetInt() const { return _int;}

    const char* GetIdentifier() const { return _identifier;}

    const char* GetFilename() const { return _filename; }

    int GetLine() const { return _line; }
    
private:
    void SkipWhitespace();

    void SkipComment();

    bool SkipPragma();

    const char* _source;
    const char* _filename;
    int _line;
    Token _token;
    float _float;
    int _int;
    const char* _identifier;
    int _position;
};

} // namespace hlsl
