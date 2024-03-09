#include "lexer.h"
#include <cassert>
#include <cctype>
#include <optional>
#include <stack>


using namespace yapvm;


Token::Token()
    : lexeme_{}, kind_{ ERROR }, line_{ SIZE_MAX } {}


Token::Token(const std::string &lexeme, TokenType kind, size_t line)
    : lexeme_{ lexeme }, kind_{ kind }, line_{ line } {}


const std::string &Token::lexeme() const {
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
    assert(!isdigit(source[cursor_pos]));

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


// return length of integer in symbols, 0 if not integer
size_t try_tokenize_int(const std::string &str, size_t cursor_pos) {
    size_t int_len = 0;
    while (isdigit(str[cursor_pos])) {
        int_len++;
        cursor_pos++;
        if (cursor_pos == str.size()) {
            break;
        }
    }

    return int_len;
}


// return length of float in symbols, 0 if not float
size_t try_tokenize_float(const std::string &str, size_t cursor_pos) {
    size_t float_first_part_len = try_tokenize_int(str, cursor_pos);
    if (float_first_part_len == 0) {
        return 0;
    }
    cursor_pos += float_first_part_len;

    if (str[cursor_pos] != '.') {
        return 0;
    }
    cursor_pos++;
    size_t float_second_part_len = try_tokenize_int(str, cursor_pos);
    return float_first_part_len + float_second_part_len + 1;
}


static bool isletter(char c) {
    return std::islower(c) || std::isupper(c);
}


static bool is_identifier_begin(char c) {
    return isletter(c) || c == '_';
}


static size_t try_tokenize_identifier(const std::string &str, size_t cursor_pos) {
    if (!is_identifier_begin(str[cursor_pos])) {
        return 0;
    }

    size_t len = 0;
    char curr = str[cursor_pos];

    while (isdigit(curr) || isletter(curr) || curr == '_') {
        len++;
        cursor_pos++;
        if (cursor_pos == str.size()) {
            break;
        }
        curr = str[cursor_pos];
    }

    return len;
}


static size_t try_tokenize_quot_content(const std::string &str, size_t cursor_pos) {
    if (str[cursor_pos] != '"' && str[cursor_pos] != '\'') {
        return 0;
    }
    
    if (cursor_pos == str.size()) {
        return 0;
    }
    
    char open_quot = str[cursor_pos];

    size_t len = 1;
    cursor_pos++;

    while (true) {
        if (str[cursor_pos] == open_quot && str[cursor_pos - 1] != '\\') {
            len++;
            break;
        }

        len++;
        if (str[cursor_pos] == '\n') {
            len = 0;
            break;
        }
        cursor_pos++;
        if (cursor_pos >= str.size()) {
            len = 0;
            break;
        }
    }

    return len;
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
 * integer    ::= decint | hexint
 *
 * identifier ::= (letter | "_") (letter | digit | "_")
 * letter     ::= lowercase | uppercase
 * lowercase  ::= "a" | "b" | ... | "z"
 * uppercase  ::= "A" | "B" | ... | "Z"
 * digit      ::= "0" | "1" | ... | "9"
 *
 * string     ::= "'" [^"\n" "'"]* "'" | '"' [^'\n' '"']* '"'
 */
std::vector<Token> yapvm::tokenize(const std::string &source) {
    std::vector<Token> tokens;
    size_t line = 1;
    std::stack<size_t> indents;
    indents.push(0);

    assert(source.find('\t') == std::string::npos);
    assert(source.find('\r') == std::string::npos);

    size_t line_start = 0;
    size_t pos = 0;

    while (pos < source.size()) {
        //TODO check '\r'???
        if (source[pos] == ' ') {
            pos++;
            continue;
        }
        if (source[pos] == '\n') {
            tokens.emplace_back("\\n", NEWLINE, line);
            line++;
            pos++;
            if (pos >= source.size()) {
                break;
            }
            line_start = pos;

            size_t line_indent = 0;
            while (source[pos] == ' ') {
                line_indent++;
                pos++;
            }
            if (source[pos] == '\n') { //TODO dedents !!!!
                continue; // skip empty line
            }

            if (line_indent > indents.top()) {
                indents.push(line_indent);
                tokens.emplace_back("", INDENT, line);
            } else {
                while (line_indent < indents.top()) {
                    indents.pop();
                    tokens.emplace_back("", DEDENT, line);
                }
            }
        }

        size_t possible_int_len = try_tokenize_int(source, pos);
        if (possible_int_len != 0) {
            tokens.emplace_back(source.substr(pos, possible_int_len), INT, line);
            pos += possible_int_len;
            continue;
        } else {
            size_t possible_float_len = try_tokenize_float(source, pos);
            if (possible_float_len != 0) {
                tokens.emplace_back(source.substr(pos, possible_float_len), FLOAT, line);
                pos += possible_float_len;
                continue;
            }
        }

        std::optional<Token> possible_keyword = try_parse_keyword(source, pos, line);
        if (possible_keyword.has_value()) {
            Token &keyword = possible_keyword.value();
            tokens.emplace_back(keyword);
            pos += keyword.lexeme().size();
            continue;
        }

        size_t possible_identifier_len = try_tokenize_identifier(source, pos);
        if (possible_identifier_len != 0) {
            tokens.emplace_back(source.substr(pos, possible_identifier_len), IDENTIFIER, line);
            pos += possible_identifier_len;
            continue;
        }

        size_t possible_string_len = try_tokenize_quot_content(source, pos);
        if (possible_string_len != 0) {
            tokens.emplace_back(source.substr(pos, possible_string_len), STRING, line);
            pos += possible_string_len;
            continue;
        }

        std::string lexeme;
        TokenType kind = ERROR;

        switch (source[pos]) {
        case '+':
            lexeme = "+";
            kind = PLUS;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "+=";
                    kind = PLUS_EQUAL;
                    pos++;
                }
            }
            break;
        case '-':
            lexeme = "-";
            kind = MINUS;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "-=";
                    kind = MINUS_EQUAL;
                    pos++;
                }
            }
            break;
        case '*':
            lexeme = "*";
            kind = ASTERISK;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '*') {
                    lexeme = "**";
                    kind = DOUBLE_ASTERISK;
                    pos++;
                } else if (source[pos + 1] == '=') {
                    lexeme = "*=";
                    kind = ASTERISK_EQUAL;
                    pos++;
                }
            }
            break;
        case '/':
            lexeme = "/";
            kind = SLASH;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '/') {
                    lexeme = "//";
                    kind = DOUBLE_SLASH;
                    pos++;
                } else if (source[pos + 1] == '=') {
                    lexeme = "/=";
                    kind = SLASH_EQUAL;
                    pos++;
                }
            }
            break;
        case '%':
            lexeme = "%";
            kind = MOD;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "%=";
                    kind = MOD_EQUAL;
                    pos++;
                }
            }
            break;
        case '|':
            lexeme = "|";
            kind = PIPE;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "|=";
                    kind = OR_EQUAL;
                    pos++;
                }
            }
            break;
        case '^':
            lexeme = "^";
            kind = CARET;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "^=";
                    kind = XOR_EQUAL;
                    pos++;
                }
            }
            break;
        case '&':
            lexeme = "&";
            kind = AMPERSAND;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "&=";
                    kind = AND_EQUAL;
                    pos++;
                }
            }
            break;
        case '~':
            lexeme = "~";
            kind = TILDE;
            break;
        case '<':
            lexeme = "<";
            kind = LESS;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '<') {
                    lexeme = "<<";
                    kind = LEFT_SHIFT;
                    if (pos < source.size() - 2) {
                        if (source[pos + 2] == '=') {
                            lexeme = "<<=";
                            kind = LEFT_SHIFT_EQUAL;
                            pos++;
                        }
                    }
                    pos++;
                } else if (source[pos + 1] == '=') {
                    lexeme = "<=";
                    kind = LESS_EQUAL;
                    pos++;
                }
            }
            break;
        case '>':
            lexeme = ">";
            kind = GREATER;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '>') {
                    lexeme = ">>";
                    kind = RIGHT_SHIFT;
                    if (pos < source.size() - 2) {
                        if (source[pos + 2] == '=') {
                            lexeme = ">>=";
                            kind = RIGHT_SHIFT_EQUAL;
                            pos++;
                        }
                    }
                    pos++;
                } else if (source[pos + 1] == '=') {
                    lexeme = ">=";
                    kind = GREATER_EQUAL;
                    pos++;
                }
            }
            break;
        case '=':
            lexeme = "=";
            kind = EQUALS;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "==";
                    kind = EQUAL_EQUAL;
                    pos++;
                }
            }
            break;
        case '!':
            lexeme = "!";
            kind = BANG;
            if (pos < source.size() - 1) {
                if (source[pos + 1] == '=') {
                    lexeme = "!=";
                    kind = BANG_EQUAL;
                    pos++;
                }
            }
            break;
        case '.':
            lexeme = ".";
            kind = DOT;
            break;
        case '(':
            lexeme = "(";
            kind = LEFT_PAREN;
            break;
        case ')':
            lexeme = ")";
            kind = RIGHT_PAREN;
            break;
        case '{':
            lexeme = "{";
            kind = LEFT_BRACE;
            break;
        case '}':
            lexeme = "}";
            kind = RIGHT_BRACE;
            break;
        case '[':
            lexeme = "[";
            kind = LEFT_SQ_BRACE;
            break;
        case ']':
            lexeme = "]";
            kind = RIGHT_SQ_BRACE;
            break;
        case ',':
            lexeme = ",";
            kind = COMMA;
            break;
        case ':':
            lexeme = ":";
            kind = COLON;
            break;
        case ';':
            lexeme = ';';
            kind = SEMICOLON;
            break;
        }

        pos++;
        tokens.emplace_back(lexeme, kind, line);
        assert(kind != ERROR); // currently for debugging
    }

    while (indents.size() > 1) {
        indents.pop();
        tokens.emplace_back("", DEDENT, line);
    }
    tokens.emplace_back("", END_OF_FILE, line);

    return tokens;
}


bool yapvm::operator==(const Token &l, const Token &r) {
    if (l.kind() != r.kind() || l.line() != r.line()) {
        return false;
    }
    return l.lexeme() == r.lexeme();
}


std::string yapvm::to_string(TokenType tt) {
    switch (tt) {
    case PRINT:
        return "PRINT";
    case THREAD:
        return "THREAD";
    case CLASS:
        return "CLASS";
    case DEF:
        return "DEF";
    case RETURN:
        return "RETURN";
    case WHILE:
        return "WHILE";
    case BREAK:
        return "BREAK";
    case CONTINUE:
        return "CONTINUE";
    case IF:
        return "IF";
    case ELIF:
        return "ELIF";
    case ELSE:
        return "ELSE";
    case TRUE:
        return "TRUE";
    case FALSE:
        return "FALSE";
    case NONE:
        return "NONE";
    case AND:
        return "AND";
    case OR:
        return "OR";
    case NOT:
        return "NOT";
    case PASS:
        return "PASS";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case ASTERISK:
        return "ASTERISK";
    case DOUBLE_ASTERISK:
        return "DOUBLE_ASTERISK";
    case SLASH:
        return "SLASH";
    case DOUBLE_SLASH:
        return "DOUBLE_SLASH";
    case MOD:
        return "MOD";
    case PIPE:
        return "PIPE";
    case CARET:
        return "CARET";
    case AMPERSAND:
        return "AMPERSAND";
    case TILDE:
        return "TILDE";
    case LEFT_SHIFT:
        return "LEFT_SHIFT";
    case RIGHT_SHIFT:
        return "RIGHT_SHIFT";
    case LESS:
        return "LESS";
    case LESS_EQUAL:
        return "LESS_EQUAL";
    case GREATER:
        return "GREATER";
    case GREATER_EQUAL:
        return "GREATER_EQUAL";
    case EQUALS:
        return "EQUALS";
    case EQUAL_EQUAL:
        return "EQUAL_EQUAL";
    case BANG:
        return "BANG";
    case BANG_EQUAL:
        return "BANG_EQUAL";
    case PLUS_EQUAL:
        return "PLUS_EQUAL";
    case MINUS_EQUAL:
        return "MINUS_EQUAL";
    case ASTERISK_EQUAL:
        return "ASTERISK_EQUAL";
    case SLASH_EQUAL:
        return "SLASH_EQUAL";
    case MOD_EQUAL:
        return "MOD_EQUAL";
    case AND_EQUAL:
        return "AND_EQUAL";
    case OR_EQUAL:
        return "OR_EQUAL";
    case XOR_EQUAL:
        return "XOR_EQUAL";
    case LEFT_SHIFT_EQUAL:
        return "LEFT_SHIFT_EQUAL";
    case RIGHT_SHIFT_EQUAL:
        return "RIGHT_SHIFT_EQUAL";
    case DOT:
        return "DOT";
    case LEFT_PAREN:
        return "LEFT_PAREN";
    case RIGHT_PAREN:
        return "RIGHT_PAREN";
    case LEFT_BRACE:
        return "LEFT_BRACE";
    case RIGHT_BRACE:
        return "RIGHT_BRACE";
    case LEFT_SQ_BRACE:
        return "LEFT_SQ_BRACE";
    case RIGHT_SQ_BRACE:
        return "RIGHT_SQ_BRACE";
    case COMMA:
        return "COMMA";
    case COLON:
        return "COLON";
    case SEMICOLON:
        return "SEMICOLON";
    case INDENT:
        return "INDENT";
    case DEDENT:
        return "DEDENT";
    case NEWLINE:
        return "NEWLINE";
    case END_OF_FILE:
        return "END_OF_FILE";
    case INT:
        return "INT";
    case FLOAT:
        return "FLOAT";
    case NAME:
        return "NAME";
    case STRING:
        return "STRING";
    case IDENTIFIER:
        return "IDENTIFIER";
    case ERROR:
        return "ERROR";
    default:
        assert(false);
    }
}