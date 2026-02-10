#pragma once

#include <string>
#include "./TokenType.h"

class Token {
    private:
        TokenType type;
        std::string lex;
    
    public:
        Token(TokenType type, std::string lex) : type(type), lex(lex) {};
        ~Token() = default;
        
        TokenType getTokenType() { return type; }
        std::string token() { return lex; }
};