#pragma once

#include "AST.hpp"

enum class Expected{
    IDENTIFIER,
    VARIABLE,
    COLON,
    TYPE, //* i8 -> i64 , u8 -> u64 , f32, f64
    EQUAL,
    LITERAL,
    RPARA,
    LPARA,
    START,
    OPERATOR //* + , - , * , /
};

std::unordered_map<TokenType, std::string> token_word;

void report_error_to_diagnostics(const std::vector<Token>& line, const std::vector<Expected>& expected_vector, const Token& got, std::string file_name);

bool is_type(TokenType type);

bool is_expected(const std::vector<Expected>& expected_vector, const TokenType& have);

void LET(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector);
void IDENT(const Token& token, std::string& ident, std::vector<Expected>& expected_vector);
void COLON(std::vector<Expected>& expected_vector);
void TYPE(const Token& token, std::vector<Expected>& expected_vector, TokenType& dec_type);
void EQUAL(std::vector<Expected>& expected_vector);
void LITERAL(std::vector<Token>& expr, const Token& token);
void PRINT(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector);
void RPARA(std::vector<Expected>& expected_vector);

void call_token(std::vector<Token>& expr, const Token& token, AST1_Type& type, std::string& ident, std::vector<Expected>& expected_vector, TokenType& dec_type);

AST1_NODE AST1(const std::vector<Token>& line, std::string file_name);