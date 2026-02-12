#include "Lexer.h"

Lexer::Lexer(const std::string &query) : query_(std::move(query)) {}

bool Lexer::isAtEnd() { return currPos_ == query_.end(); }

char Lexer::peek() { return *currPos_; }

std::vector<Token> Lexer::getTokens() { return tokens_; }

bool Lexer::isDigit(char ch) { return ch >= '0' && ch <= '9'; }

void Lexer::consume(TokenType type, std::string word) { tokens_.push_back(Token(type,word)); }

void Lexer::move() { 
    if(isAtEnd()) return;
    currPos_++; 
}

void Lexer::tokenize() {
    while(!isAtEnd()) scanToken();
    consume(TokenType::END_OF_FILE,"");
    return;
}

std::string Lexer::getString(std::function<bool(char)> stop) {
    std::string word = "";
    while(!isAtEnd() && stop(peek()) && peek() != ';') {
        word += peek();
        move();
    }
    return word;
}

std::string Lexer::getString(char stop) {
    std::string word = "";
    while(!isAtEnd() && peek() != stop && peek() != ';') {
        word += peek();
        move();
    }
    if(peek() != stop) throw std::runtime_error(std::string("\033[31mERROR: Expected ' ") + stop + " '.\033[0m");
    if(isAtEnd()) throw std::runtime_error(std::string("\033[31mERROR: Expected ' ") + stop + " '.\033[0m");
    return word;
}


TokenType Lexer::getTokenType(std::string &word) {
    for(auto& c : word) c = toupper(c);
    if(word == "INSERT") return TokenType::INSERT;
    else if(word == "SEARCH") return TokenType::SEARCH;
    else if(word == "DELETE") return TokenType::DELETE;
    else if(word == "UPDATE") return TokenType::UPDATE;
    else throw std::runtime_error(std::string("\033[31mERROR: Unexpected token ") + word + ".\033[0m");
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
                if(isdigit(peek())) consume(TokenType::ID,getString(isdigit));
                else { 
                    w = getString(isalpha);
                    TokenType type = getTokenType(w);
                    consume(type ,w);
                }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}



