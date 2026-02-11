#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include "./Node/ASTNode.h"
#include "../Lexer/Token/Token.h"

class Parser {
    private:
        std::vector<Token> tokens;
        std::unique_ptr<Expr> root = nullptr;
        std::vector<Token>::iterator currPos = tokens.begin();

        std::unique_ptr<Expr> parseExpression() ;
        std::unique_ptr<Expr> parseLeft();
        std::unique_ptr<Expr> parseRight();
        void move();
        bool isMatch(TokenType type);
        TokenType peek();
        bool isAtEnd();
        std::string consume();
        void printAST(Expr* node);
        std::string tokenType(TokenType type);

    public:
        Parser(std::vector<Token> tokens);
        ~Parser() = default;

        void parse();
};