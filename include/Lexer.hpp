#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>

enum class TokenType{

    // Literals
    INT,
    FLOAT,
    STR,

    IDENTIFIER,

    // Types
    I8,
    U8,
    I16,
    U16,
    I32,
    U32,
    I64,
    U64,
    F32,
    F64,

    // Punctuation
    LBRAC,    // '{'
    RBRAC,   // '}'
    LPARA,  // '('
    RPARA, // ')'
    COLON,

    // Operators
    EQUAL,
    ADD,
    SUB,
    MUL,
    DIV,

    // Keywords
    LET,
    PRINT,

    NULL_
};

struct lexical_token{
    size_t line;
    std::string string;
    size_t org_start_pos;
    bool is_string;
    std::string org_line;
};

struct Token{
    size_t org_start_pos;
    size_t line_num;
    std::string org_word;
    std::string org_line;
    TokenType type;
};

struct is_num_is_decimal{
    bool valid;
    bool decimal;
};

is_num_is_decimal isdigitstr(std::string str);
bool only_space(std::string str);
std::string trim(std::string& s);
std::vector<std::vector<lexical_token>> Stream(std::vector<std::string> lines);
std::vector<Token> Tokenize(std::vector<lexical_token> line);