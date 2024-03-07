#include "lexer.h"
#include <cassert>
#include <cctype>
#include <optional>

using namespace yapvm;


Token::Token()
    : lexeme_{}, kind_{ ERROR }, line_{ SIZE_MAX } {}


Token::Token(const std::string &lexeme, TokenType kind, size_t line)
    : lexeme_{ lexeme }, kind_{ kind }, line_{ line } {}


const std::string &Token::lexeme() {
    return lexeme_;
}


TokenType Token::kind() const {
    return kind_;
}


size_t Token::line() const {
    return line_;
}


static bool try_tokenize_keyword(const std::string &source, const std::string &keyword, size_t cursor_pos) {
    if (keyword.size() > source.size() - cursor_pos) {
        return false;
    }

    bool match = true;
    for (size_t i = 0; i < keyword.size(); i++) {
        if (keyword[i] != source[i + cursor_pos]) {
            match = false;
            break;
        }
    }
    return match;
}


static std::optional<Token> try_parse_keyword(const std::string &source, size_t cursor_pos, size_t line) {
    if (!(std::islower(source[cursor_pos]) || std::isupper(source[cursor_pos]))) {
        return std::nullopt;
    }

    size_t symbols_til_source_end = source.size() - cursor_pos;
    switch (symbols_til_source_end) {
    default:

    case 8: // continue
        if (try_tokenize_keyword(source, "continue", cursor_pos)) {
            return { { source.substr(cursor_pos, 8), CONTINUE, line } };
        }
    case 7:

    case 6: // return, Thread
        if (try_tokenize_keyword(source, "return", cursor_pos)) {
            return { { source.substr(cursor_pos, 6), RETURN, line } };
        }
        if (try_tokenize_keyword(source, "Thread", cursor_pos)) {
            return { { source.substr(cursor_pos, 6), THREAD, line } };
        }
    case 5: // print, False, while, break, class
        if (try_tokenize_keyword(source, "print", cursor_pos)) {
            return { { source.substr(cursor_pos, 5), PRINT, line } };
        }
        if (try_tokenize_keyword(source, "False", cursor_pos)) {
            return { { source.substr(cursor_pos, 5), FALSE, line } };
        }
        if (try_tokenize_keyword(source, "while", cursor_pos)) {
            return { { source.substr(cursor_pos, 5), WHILE, line } };
        }
        if (try_tokenize_keyword(source, "break", cursor_pos)) {
            return { { source.substr(cursor_pos, 5), BREAK, line } };
        }
        if (try_tokenize_keyword(source, "class", cursor_pos)) {
            return { { source.substr(cursor_pos, 5), CLASS, line } };
        }
    case 4: // True, None, elif, else, pass
        if (try_tokenize_keyword(source, "True", cursor_pos)) {
            return { { source.substr(cursor_pos, 4), TRUE, line } };
        }
        if (try_tokenize_keyword(source, "None", cursor_pos)) {
            return { { source.substr(cursor_pos, 4), NONE, line } };
        }
        if (try_tokenize_keyword(source, "elif", cursor_pos)) {
            return { { source.substr(cursor_pos, 4), ELIF, line } };
        }
        if (try_tokenize_keyword(source, "else", cursor_pos)) {
            return { { source.substr(cursor_pos, 4), ELSE, line } };
        }
        if (try_tokenize_keyword(source, "pass", cursor_pos)) {
            return { { source.substr(cursor_pos, 4), PASS, line } };
        }
    case 3: // and, not, def
        if (try_tokenize_keyword(source, "and", cursor_pos)) {
            return { { source.substr(cursor_pos, 3), AND, line } };
        }
        if (try_tokenize_keyword(source, "not", cursor_pos)) {
            return { { source.substr(cursor_pos, 3), NOT, line } };
        }
        if (try_tokenize_keyword(source, "def", cursor_pos)) {
            return { { source.substr(cursor_pos, 3), DEF, line } };
        }
    case 2: // or, if
        if (try_tokenize_keyword(source, "or", cursor_pos)) {
            return { { source.substr(cursor_pos, 2), OR, line } };
        }
        if (try_tokenize_keyword(source, "if", cursor_pos)) {
            return { { source.substr(cursor_pos, 2), IF, line } };
        }
    }

    return std::nullopt;
}

/*
 * Categories sorted by matching priority
 *
 * keyword    ::= "continue" | "return" | "Thread"
 *              | "print"    | "False"  | "while"
 *              | "break"    | "class"  | "True"
 *              | "None"     | "elif"   | "else"
 *              | "pass"     | "and"    | "not"
 *              | "def"      | "or"     | "if"
 *
 * number     ::= integer | float
 * integer    ::= decint | octint | hexint
 *
 * identifier ::= (letter | "_") (letter | digit | "_")
 * letter     ::= lowercase | uppercase
 * lowercase  ::= "a" | "b" | ... | "z"
 * uppercase  ::= "A" | "B" | ... | "Z"
 * digit      ::= "0" | "1" | ... | "9"
 *
 * string     ::= "'" [^"\n""'"]* "'" | '"' [^'\n' '"']* '"'
 */
std::vector<Token> yapvm::tokenize(const std::string &source) {
    size_t line = 0;
    for (size_t cursor = 0; cursor < source.size(); cursor++) {

    }

    return {};
}