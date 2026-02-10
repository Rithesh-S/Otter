#include "Lexer.h"

Lexer::Lexer(const std::string &query) : query_(std::move(query)) {}

bool Lexer::isAtEnd() { return currPos_ == query_.end(); }

char Lexer::peek() { return *currPos_; }

std::vector<Token> Lexer::getTokens() { return tokens_; }

bool Lexer::isDigit(char ch) { return ch > '0' && ch < '9'; }

void Lexer::consume(TokenType type, std::string word) { tokens_.push_back(Token(type,word)); }

void Lexer::move() { 
    if(isAtEnd()) return;
    currPos_++; 
}

std::string Lexer::getString(char stop) {
    std::string word = "";
    while(!isAtEnd() && peek() != stop && peek() != ';') {
        word += peek();
        move();
    }
    if(isAtEnd()) throw std::runtime_error(std::string("\033[31mERROR: Expected '") + stop + "'.\033[0m");
    return word;
}

void Lexer::tokenize() {
    while(!isAtEnd()) {
        scanToken();
    }
    consume(TokenType::END_OF_FILE,"");
    return;
}

std::string Lexer::tokenType(TokenType type) {             //helper
    if(type == TokenType::INSERT) return "INSERT";
    else if(type == TokenType::SEARCH) return "SEARCH";
    else if(type == TokenType::DELETE) return "DELETE";
    else if(type == TokenType::UPDATE) return "UPDATE";
    else if(type == TokenType::ID) return "ID";
    else if(type == TokenType::DATA) return "DATA";
    else if(type == TokenType::END_OF_FILE) return "EOF";
    else return "";
}

void Lexer::scanToken() {
    try {
        std::string w;
        switch (peek()) {
            case '\'':
                move();
                consume(TokenType::DATA ,getString('\''));
                move();
                break;
                
            case '\"':
                move();
                consume(TokenType::DATA ,getString('\"'));
                move();
                break;

            case ' ':
                move();
                break;
            
            case ';':
                while(!isAtEnd()) move();
                break;

            default:
                if(isDigit(peek())) consume(TokenType::ID,getString(' '));
                else { 
                    w = getString(' ');
                    consume(getTokenType(w),w);
                }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}

TokenType Lexer::getTokenType(const std::string &word) {
    if(word == "INSERT") return TokenType::INSERT;
    else if(word == "SEARCH") return TokenType::SEARCH;
    else if(word == "DELETE") return TokenType::DELETE;
    else if(word == "UPDATE") return TokenType::UPDATE;
    else throw std::runtime_error(std::string("\033[31mERROR: Unexpected token ") + word + ".\033[0m");
}



