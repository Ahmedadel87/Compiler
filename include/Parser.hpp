#pragma once

#include "AST.hpp"

bool is_operator(TokenType type);

bool is_type(TokenType type);

void print_expr(const ExprNode& node, int indent = 0);

ExprNode Parse_expression(const std::vector<Token>& expr);

class Parser{
    private:
        std::vector<Token> tokens;
        int pos = 0;

    public:
        Parser(std::vector<Token> user_tokens) { tokens = user_tokens; }
        Token& current(){ 
            static Token null_token = {0, 0, "", "", TokenType::NULL_};
            if(tokens.empty()) return null_token;
            return tokens[pos]; 
        }
        Token& peek(int offset = 1){ 
            if(pos + offset >= (int)tokens.size()) return tokens.back();
            return tokens[pos + offset]; 
        }
        Token& consume(int offset = 1){ 
            pos += offset;
            if(pos >= (int)tokens.size()) return tokens.back();
            return tokens[pos]; 
        }
        bool check(TokenType token, int offset = 1){ return (token == peek(offset).type) ? true : false; }
        std::vector<Token> left(){ return std::vector<Token>(tokens.begin() + pos + 1, tokens.end()); }
        std::vector<Token>& line(){ return tokens; }
};

AST_NODE let_dec(Parser& parser);

void AST(Parser& parser);