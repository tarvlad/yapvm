#include "lexer.h"
#include <print>
#include <regex>
#include "lexer.h"
#include "utils.h"

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
        { "def", DEF, 14 }
        //TODO
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