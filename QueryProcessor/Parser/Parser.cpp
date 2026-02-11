#include "Parser.h"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

void Parser::parse() {
    try {
        root = parseExpression();
        if(!isAtEnd()) {
            root = nullptr;
            throw std::runtime_error("\033[31mERROR: Invalid Syntax.\033[0m");
        }
    } catch(const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }
    printAST(root.get());
}

std::unique_ptr<Expr> Parser::parseExpression() {
    try {    
        if(isMatch(TokenType::INSERT) || isMatch(TokenType::UPDATE)) {
            TokenType data = peek();
            consume();
            auto left = parseLeft();
            auto right = parseRight();
            return std::move(std::make_unique<Binary>(std::move(left), data, std::move(right)));
        } else if(isMatch(TokenType::SEARCH) || isMatch(TokenType::DELETE)) {
            TokenType data = peek();
            consume();
            auto left = parseLeft();
            return std::move(std::make_unique<Unary>(std::move(left), data));
        } else {
            throw std::runtime_error("\033[31mERROR: Unexpected token.\033[0m");
        }
    } catch(...) { throw; }
}

std::unique_ptr<Expr> Parser::parseLeft() {
    if(isMatch(TokenType::ID)) {
        TokenType type = peek();
        std::string data = consume();
        return std::move(std::make_unique<Literal>(type, data));
    } else {
        throw std::runtime_error("\033[31mERROR: Unexpected token.\033[0m");
    }
}

std::unique_ptr<Expr> Parser::parseRight() {
    if(isMatch(TokenType::DATA)) {
        TokenType type = peek();
        std::string data = consume();
        return std::move(std::make_unique<Literal>(type, data));
    } else {
        throw std::runtime_error("\033[31mERROR: Unexpected token.\033[0m");
    }
}

TokenType Parser::peek() { return currPos -> getTokenType(); }

bool Parser::isAtEnd() { return currPos -> getTokenType() == TokenType::END_OF_FILE; }

bool Parser::isMatch(TokenType type) { return peek() == type; }

void Parser::move() {
    if(isAtEnd()) return;
    currPos++;
}

std::string Parser::consume() {
    std::string data = currPos -> token();
    move();
    return data;
}

void Parser::printAST(Expr* node) {
    if(!node) {
        std::cout << "Empty Tree" << std::endl;
        return;
    }

    if(Literal* literal = dynamic_cast<Literal*>(node)) {
        std::cout << literal -> data << std::endl;
        return;
    }

    if(Unary* unary = dynamic_cast<Unary*>(node)) {
        std::cout << tokenType(unary -> type) << std::endl;
        printAST(unary -> left.get());
    }
    
    if(Binary* binary = dynamic_cast<Binary*>(node)) {
        std::cout << tokenType(binary -> type) << std::endl;
        printAST(binary -> left.get());
        printAST(binary -> right.get());
    }
}

std::string Parser::tokenType(TokenType type) {             //helper
    if(type == TokenType::INSERT) return "INSERT";
    else if(type == TokenType::SEARCH) return "SEARCH";
    else if(type == TokenType::DELETE) return "DELETE";
    else if(type == TokenType::UPDATE) return "UPDATE";
    else if(type == TokenType::ID) return "ID";
    else if(type == TokenType::DATA) return "DATA";
    else if(type == TokenType::END_OF_FILE) return "EOF";
    else return "";
}