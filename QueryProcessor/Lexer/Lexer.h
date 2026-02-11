#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <sstream>
#include "./Token/Token.h"

class Lexer {
    private:
        std::string query_;
        std::vector<Token> tokens_;
        std::string::iterator currPos_ = query_.begin();

        char peek();
        void move();
        bool isAtEnd();
        void scanToken();
        bool isDigit(char ch);
        void consume(TokenType type, std::string word);

        std::string getString(char stop = ' ');
        TokenType getTokenType(std::string &word);

    public:
        Lexer(const std::string &query);
        ~Lexer() = default;

        void tokenize();
        std::vector<Token> getTokens();
        
};