#pragma once

#include "Lexer.hpp"
#include <variant>

/*
DECLARATION, ASSIGNMENT, PRINT
*/
enum class AST1_Type{
    DECLARATION,
    ASSIGNMENT,
    PRINT,
    NULL_
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

struct LiteralExpr { Token value; };

struct ExprNode;

struct BinaryExpr {
    Token op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
};

struct ExprNode {
    std::variant<LiteralExpr, BinaryExpr> node;
};

struct Declaration_Node{
    std::string identifier;
    TokenType type; //* Only TokenTypes that are types will be assigned here, ex: i32
    ExprNode expr;
};

using Node = std::variant<Declaration_Node>;

struct AST1_NODE{
    AST1_Type type;
    Node node;
    bool iterable;
    bool const_;
    const std::vector<Token>* reference;
};