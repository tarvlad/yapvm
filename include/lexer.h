#pragma once

#include <string>
#include <cstddef>
#include <vector>


namespace yapvm {


enum TokenType {
    // IO
    PRINT,

    // THREADS
    THREAD, 

    // DEF
    CLASS, DEF, 
    
    // CFG
    RETURN, 
    WHILE, 
    BREAK, 
    CONTINUE, 
    IF, 
    ELIF, 
    ELSE,

    // CONST
    TRUE, 
    FALSE, 
    NONE,

    // LOGIC
    AND, 
    OR, 
    NOT,

    // DUMMY
    PASS,

    // OPERATORS
    PLUS, 
    MINUS, 
    ASTERISK, 
    DOUBLE_ASTERISK,
    SLASH, 
    DOUBLE_SLASH, 
    MOD,
    PIPE, 
    CARET, 
    AMPERSAND, 
    TILDE, 
    LEFT_SHIFT, 
    RIGHT_SHIFT, 
    LESS, 
    LESS_EQUAL,
    GREATER, 
    GREATER_EQUAL,
    EQUALS, 
    EQUAL_EQUAL,
    BANG, 
    BANG_EQUAL,
    PLUS_EQUAL, 
    MINUS_EQUAL,
    ASTERISK_EQUAL,
    SLASH_EQUAL,
    MOD_EQUAL,
    AND_EQUAL,
    OR_EQUAL,
    XOR_EQUAL,
    LEFT_SHIFT_EQUAL,
    RIGHT_SHIFT_EQUAL,
    DOT,

    // PUNCT
    LEFT_PAREN, 
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_SQ_BRACE,
    RIGHT_SQ_BRACE,
    COMMA, 
    COLON, 
    SEMICOLON,
    INDENT,
    DEDENT,
    NEWLINE,
    END_OF_FILE,

    // LITERALS
    INT, FLOAT, NAME, STRING,

    IDENTIFIER,

    ERROR
};


std::string to_string(TokenType tt);


class Token {
    std::string lexeme_;
    TokenType kind_;
    size_t line_;

public:
    Token();

    Token(const std::string &lexeme, TokenType kind, size_t line);

    const std::string &lexeme() const;
    
    TokenType kind() const;

    size_t line() const;

    friend bool operator==(const Token &l, const Token &r);
};


std::vector<Token> tokenize(const std::string &source);


}