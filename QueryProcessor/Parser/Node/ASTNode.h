#pragma once

#include <memory>
#include <string>
#include "../../Lexer/Token/TokenType.h"

struct Expr {
    virtual ~Expr() = default;
};

struct Binary : public Expr {
    std::unique_ptr<Expr> left;
    TokenType type;
    std::unique_ptr<Expr> right;

    Binary(std::unique_ptr<Expr> left, TokenType type, std::unique_ptr<Expr> right) :
        left(std::move(left)), type(type), right(std::move(right)) {}
};

struct Unary : public Expr {
    std::unique_ptr<Expr> left;
    TokenType type;

    Unary(std::unique_ptr<Expr> left, TokenType type) :
        left(std::move(left)), type(type) {}
};

struct Literal : public Expr {
    TokenType type;
    std::string data;

    Literal(TokenType type, std::string data) :
        type(type), data(data) {}
};

