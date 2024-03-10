#include "lexer.h"
#include <print>
#include <regex>
#include "lexer.h"
#include "utils.h"

using namespace yapvm::lexer;
using namespace yapvm;


int main() {
    std::string test_source = read_file("../test_resources/lexer_test_all.py");
    std::vector<Token> tokens = tokenize(test_source);

    std::vector<Token> expected_tokens = {
        { "print", PRINT, 1 },
        { "(", LEFT_PAREN, 1 },
        { "abcdef_gh", IDENTIFIER, 1 },
        { ")", RIGHT_PAREN, 1 },
        { "\\n", NEWLINE, 1 },

        { "\\n", NEWLINE, 2 },
        
        { "Thread", THREAD, 3 },
        { "(", LEFT_PAREN, 3 },
        { "a", IDENTIFIER, 3 },
        { ",", COMMA, 3 }, 
        { "b", IDENTIFIER, 3 },
        { ")", RIGHT_PAREN, 3 },
        { "\\n", NEWLINE, 3 },

        { "\\n", NEWLINE, 4 },

        { "class", CLASS, 5 },
        { "Foo", IDENTIFIER, 5 },
        { ":", COLON, 5 },
        { "\\n", NEWLINE, 5 },
        
        { "", INDENT, 6 },
        { "def", DEF, 6 },
        { "__init__", IDENTIFIER, 6 },
        { "(", LEFT_PAREN, 6 },
        { "self", IDENTIFIER, 6 },
        { ",", COMMA, 6 },
        { "foo", IDENTIFIER, 6 },
        { ")", RIGHT_PAREN, 6 },
        { ":", COLON, 6 },
        { "\\n", NEWLINE, 6 },

        { "", INDENT, 7 },
        { "self", IDENTIFIER, 7 },
        { ".", DOT, 7 },
        { "foo", IDENTIFIER, 7 },
        { "=", EQUALS, 7 },
        { "foo", IDENTIFIER, 7},
        { "\\n", NEWLINE, 7 },

        { "", DEDENT, 8 },
        { "def", DEF, 8 },
        { "f", IDENTIFIER, 8 },
        { "(", LEFT_PAREN, 8 },
        { "x", IDENTIFIER, 8 },
        { ",", COMMA, 8 },
        { "y", IDENTIFIER, 8 },
        { ",", COMMA, 8 },
        { "z", IDENTIFIER, 8 },
        { ")", RIGHT_PAREN, 8 },
        { ":", COLON, 8 },
        { "\\n", NEWLINE, 8 },

        { "", INDENT, 9 },
        { "print", PRINT, 9 },
        { "(", LEFT_PAREN, 9 },
        { "x", IDENTIFIER, 9 },
        { ",", COMMA, 9 },
        { "y", IDENTIFIER, 9 },
        { ",", COMMA, 9 },
        { "z", IDENTIFIER, 9 },
        { ",", COMMA, 9 },
        { "foo", IDENTIFIER, 9 },
        { ")", RIGHT_PAREN, 9 },
        { "\\n", NEWLINE, 9 },
        
        { "\\n", NEWLINE, 10 },

        { "", DEDENT, 11 },
        { "def", DEF, 11 },
        { "g", IDENTIFIER, 11 },
        { "(", LEFT_PAREN, 11 },
        { "self", IDENTIFIER, 11 },
        { ",", COMMA, 11 },
        { "_name__n", IDENTIFIER, 11 },
        { "=", EQUALS, 11 },
        { "\"str\"", STRING, 11 },
        { ")", RIGHT_PAREN, 11 },
        { ":", COLON, 11 },
        { "\\n", NEWLINE, 11 },

        { "", INDENT, 12 },
        { "return", RETURN, 12 },
        { "f", IDENTIFIER, 12 },
        { "(", LEFT_PAREN, 12 },
        { "_name__n", IDENTIFIER, 12 },
        { ",", COMMA, 12 },
        { "self", IDENTIFIER, 12 },
        { ".", DOT, 12 },
        { "foo", IDENTIFIER, 12 },
        { ",", COMMA, 12 },
        { "b", IDENTIFIER, 12 },
        { ",", COMMA, 12 },
        { "c", IDENTIFIER, 12 },
        { ")", RIGHT_PAREN, 12 },
        { "\\n", NEWLINE, 12 },

        { "\\n", NEWLINE, 13 },

        { "", DEDENT, 14 },
        { "", DEDENT, 14 },
        { "def", DEF, 14 },
        { "f_0_1_", IDENTIFIER, 14 },
        { "(", LEFT_PAREN, 14 },
        { "a", IDENTIFIER, 14 },
        { ",", COMMA, 14 },
        { "b", IDENTIFIER, 14 },
        { ",", COMMA, 14 },
        { "c", IDENTIFIER, 14 },
        { ")", RIGHT_PAREN, 14 },
        { ":", COLON, 14 },
        { "\\n", NEWLINE, 14 },

        { "", INDENT, 15 },
        { "d", IDENTIFIER, 15 },
        { "=", EQUALS, 15 },
        { "c", IDENTIFIER, 15 },
        { "\\n", NEWLINE, 15 },

        { "f", IDENTIFIER, 16 },
        { "=", EQUALS, 16 },
        { "a", IDENTIFIER, 16 },
        { "\\n", NEWLINE, 16 },

        { "g", IDENTIFIER, 17 },
        { "=", EQUALS, 17 },
        { "f", IDENTIFIER, 17 },
        { "^", CARET, 17 },
        { "(", LEFT_PAREN, 17 },
        { "f", IDENTIFIER, 17 },
        { "+", PLUS, 17 },
        { "1", INT, 17 },
        { ")", RIGHT_PAREN, 17 },
        { "\\n", NEWLINE, 17 },

        { "return", RETURN, 18 },
        { "a", IDENTIFIER, 18 },
        { "+", PLUS, 18 },
        { "b", IDENTIFIER, 18 },
        { "*", ASTERISK, 18 },
        { "c", IDENTIFIER, 18 },
        { "/", SLASH, 18 },
        { "d", IDENTIFIER, 18 },
        { "%", MOD, 18 },
        { "f", IDENTIFIER, 18 },
        { "**", DOUBLE_ASTERISK, 18},
        { "g", IDENTIFIER, 18 },
        { "\\n", NEWLINE, 18 },

        { "\\n", NEWLINE, 19 },

        { "", DEDENT, 20 },
        { "Thread", THREAD, 20 },
        { ".", DOT, 20 },
        { "foo", IDENTIFIER, 20 },
        { "(", LEFT_PAREN, 20 },
        { "bar", IDENTIFIER, 20 },
        { "=", EQUALS, 20 },
        { "baz", IDENTIFIER, 20 },
        { ",", COMMA, 20 },
        { "function", IDENTIFIER, 20 },
        { ")", RIGHT_PAREN, 20 },
        { "\\n", NEWLINE, 20 },

        { "\\n", NEWLINE, 21 },
        
        { "a", IDENTIFIER, 22 },
        { "=", EQUALS, 22 },
        { "42", INT, 22 },
        { "<<", LEFT_SHIFT, 22 },
        { "11", INT, 22 },
        { "\\n", NEWLINE, 22 },

        { "b", IDENTIFIER, 23 },
        { "=", EQUALS, 23 },
        { "43", INT, 23 },
        { ">>", RIGHT_SHIFT, 23 },
        { "12", INT, 23 },
        { "\\n", NEWLINE, 23 },

        { "\\n", NEWLINE, 24 },

        { "c", IDENTIFIER, 25 },
        { "=", EQUALS, 25 },
        { "sqrt", IDENTIFIER, 25 },
        { "(", LEFT_PAREN, 25 },
        { "32.3", FLOAT, 25 },
        { ")", RIGHT_PAREN, 25},
        { "\\n", NEWLINE, 25 },

        { "\\n", NEWLINE, 26 },

        { "d", IDENTIFIER, 27},
        { "=", EQUALS, 27 },
        { "{", LEFT_BRACE, 27 },
        { "}", RIGHT_BRACE, 27 },
        { "\\n", NEWLINE, 27},

        { "__e", IDENTIFIER, 28 },
        { "=", EQUALS, 28 },
        { "[", LEFT_SQ_BRACE, 28 },
        { "]", RIGHT_SQ_BRACE, 28 },
        { "\\n", NEWLINE, 28 },

        { "\\n", NEWLINE, 29 },

        { "if", IF, 30 },
        { "a", IDENTIFIER, 30 },
        { "==", EQUAL_EQUAL, 30 },
        { "b", IDENTIFIER, 30 },
        { ":", COLON, 30 },
        { "\\n", NEWLINE, 30 },

        { "", INDENT, 31 },
        { "c", IDENTIFIER, 31 },
        { "=", EQUALS, 31 }, 
        { "d", IDENTIFIER, 31 },
        { "\\n", NEWLINE, 31 },

        { "", DEDENT, 32 },
        { "elif", ELIF, 32 },
        { "__e", IDENTIFIER, 32 },
        { ">=", GREATER_EQUAL, 32 },
        { "d", IDENTIFIER, 32 },
        { ":", COLON, 32 },
        { "\\n", NEWLINE, 32 },

        { "", INDENT, 33 },
        { "f", IDENTIFIER, 33 },
        { "=", EQUALS, 33 },
        { "32", INT, 33 },
        { "\\n", NEWLINE, 33 },

        { "", DEDENT, 34 },
        { "else", ELSE, 34 },
        { ":", COLON, 34 },
        { "\\n", NEWLINE, 34 },

        { "", INDENT, 35 },
        { "print", PRINT, 35 },
        { "(", LEFT_PAREN, 35 },
        { "42", INT, 35 },
        { ")", RIGHT_PAREN, 35 },
        { "\\n", NEWLINE, 35 },

        { "\\n", NEWLINE, 36 },

        { "", DEDENT, 37 },
        { "str1", IDENTIFIER, 37 },
        { "=", EQUALS, 37 },
        { "\"string in quots, with \\\" and '\"", STRING, 37 },
        { "\\n", NEWLINE, 37 },

        { "str2", IDENTIFIER, 38 },
        { "=", EQUALS, 38 },
        { "'string in other quots'", STRING, 38 },
        { "\\n", NEWLINE, 38 },

        { "\\n", NEWLINE, 39 },
        
        { "c", IDENTIFIER, 40 },
        { "*=", ASTERISK_EQUAL, 40 },
        { "3", INT, 40 },
        { "\\n", NEWLINE, 40 },

        { "c", IDENTIFIER, 41 },
        { ">>=", RIGHT_SHIFT_EQUAL, 41 },
        { "4", INT, 41 },
        { "\\n", NEWLINE, 41 },

        { "c", IDENTIFIER, 42 },
        { "<<=", LEFT_SHIFT_EQUAL, 42 },
        { "c", IDENTIFIER, 42 },
        { "\\n", NEWLINE, 42 },

        { "\\n", NEWLINE, 43 },

        { "if", IF, 44 },
        { "(", LEFT_PAREN, 44 },
        { "a", IDENTIFIER, 44 },
        { "==", EQUAL_EQUAL, 44 },
        { "b", IDENTIFIER, 44 },
        { ")", RIGHT_PAREN, 44 },
        { ":", COLON, 44 },
        { "\\n", NEWLINE, 44 },

        { "", INDENT, 45 },
        { "print", PRINT, 45 },
        { "(", LEFT_PAREN, 45 },
        { "42", INT, 45 },
        { ")", RIGHT_PAREN, 45 },
        { "\\n", NEWLINE, 45 },

        { "\\n", NEWLINE, 46 },

        { "\\n", NEWLINE, 47 },

        { "", DEDENT, 48 },
        { "a", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "b", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "c", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "d", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "e", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "f", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "_foo", IDENTIFIER, 48 },
        { "(", LEFT_PAREN, 48 },
        { "bar", IDENTIFIER, 48 },
        { ".", DOT, 48 },
        { "baz", IDENTIFIER, 48 },
        { "(", LEFT_PAREN, 48 },
        { ")", RIGHT_PAREN, 48 }, 
        { ",", COMMA, 48 },
        { "42", INT, 48 },
        { ")", RIGHT_PAREN, 48 },
        { "\\n", NEWLINE, 48 },

        { "\\n", NEWLINE, 49 },

        { "_f9__", IDENTIFIER, 50 },
        { ".", DOT, 50 },
        { "__fff000", IDENTIFIER, 50 },
        { "\\n", NEWLINE, 50 },

        { "\\n", NEWLINE, 51 },

        { "42", INT, 52 },
        { ";", SEMICOLON, 52 },
        { "\\n", NEWLINE, 52 },

        { "\\n", NEWLINE, 53 },
        
        { "\\n", NEWLINE, 54 },

        { "e", IDENTIFIER, 55 },
        { "|", PIPE, 55 },
        { "f", IDENTIFIER, 55 },
        { "\\n", NEWLINE, 55 },

        { "g", IDENTIFIER, 56 },
        { "&", AMPERSAND, 56 },
        { "h", IDENTIFIER, 56 },
        { "\\n", NEWLINE, 56 },

        { "e", IDENTIFIER, 57 },
        { "&=", AND_EQUAL, 57 },
        { "3", INT, 57 },
        { "\\n", NEWLINE, 57 },

        { "f", IDENTIFIER, 58 },
        { "|=", OR_EQUAL, 58 },
        { "g", IDENTIFIER, 58 },
        { "\\n", NEWLINE, 58 },

        { "3", INT, 59 },
        { "^", CARET, 59 },
        { "4", INT, 59 },
        { "**", DOUBLE_ASTERISK, 59 },
        { "5", INT, 59 },
        { "\\n", NEWLINE, 59 },

        { "\\n", NEWLINE, 60 },

        { "{", LEFT_BRACE, 61 },
        { "(", LEFT_PAREN, 61 },
        { "FOO", IDENTIFIER, 61 },
        { ",", COMMA, 61 },
        { "bar", IDENTIFIER, 61},
        { ",", COMMA, 61 },
        { "baz", IDENTIFIER, 61 },
        { ")", RIGHT_PAREN, 61 },
        { ":", COLON, 61 },
        { "3", INT, 61 },
        { ",", COMMA, 61 },
        { "\\n", NEWLINE, 61 },

        { "", INDENT, 62 },
        { "[", LEFT_SQ_BRACE, 62 },
        { "foo", IDENTIFIER, 62 },
        { ",", COMMA, 62 },
        { "bar", IDENTIFIER, 62 },
        { ",", COMMA, 62 },
        { "test", IDENTIFIER, 62 },
        { "]", RIGHT_SQ_BRACE, 62 },
        { ":", COLON, 62 },
        { "4", INT, 62 },
        { "}", RIGHT_BRACE, 62 },
        { "\\n", NEWLINE, 62 },

        { "\\n", NEWLINE, 63 },

        { "", DEDENT, 64 },
        { "", END_OF_FILE, 64 }
    };

    for (size_t i = 0; i < expected_tokens.size(); i++) {
        if (tokens[i] != expected_tokens[i]) {
            std::println("ERROR: lexer test failed");
            std::println("    Expected token [{}] with lexeme [{}] in line [{}], got [{}] with lexeme [{}] in line [{}]",
                to_string(expected_tokens[i].kind()), expected_tokens[i].lexeme(), expected_tokens[i].line(),
                to_string(tokens[i].kind()), tokens[i].lexeme(), tokens[i].line()
            );
            std::println("    Bad token index [{}]", i);
            return 1;
        }
    }

    if (tokens.size() != expected_tokens.size()) {
        std::println("ERROR: lexer test failed");
        std::println("    Number of tokens mismatch, expected [{}], got [{}]", 
            expected_tokens.size(), tokens.size()
        );
        return 1;
    }
}