#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <variant>

#include "Parser.hpp"
#include "Diagnostic.hpp"

bool is_type(TokenType type){
    if(type == TokenType::I8 || type == TokenType::U8 || type == TokenType::I16 || type == TokenType::U16 || type == TokenType::I32 || type == TokenType::U32 || type == TokenType::I64 || type == TokenType::U64 || type == TokenType::F32 || type == TokenType::F64){
        return true;
    }
    else return false; 
}

bool is_operator(TokenType type){
    if(type == TokenType::ADD || type == TokenType::SUB || type == TokenType::MUL || type == TokenType::DIV) return true;
    else return false;
}

bool is_expected(const std::vector<Expected>& expected_vector, const TokenType& have){
    for(Expected expected : expected_vector){
        switch(expected){
            case Expected::COLON:
                if(have == TokenType::COLON) return true;
                break;
            case Expected::TYPE:
                if(is_type(have)) return true;
                break;
            case Expected::IDENTIFIER:
                if(have == TokenType::IDENTIFIER) return true;
                break;
            case Expected::EQUAL:
                if(have == TokenType::EQUAL) return true;
                break;
            case Expected::LITERAL:
                if(have == TokenType::INT || have == TokenType::FLOAT || have == TokenType::STR) return true;
                break;
            case Expected::RPARA:
                if(have == TokenType::RPARA) return true;
                break;
            case Expected::LPARA:
                if(have == TokenType::LPARA) return true;
                break;
            case Expected::START:
                switch(have){
                    case TokenType::LET:
                        return true;
                        break;
                    case TokenType::PRINT:
                        return true;
                        break;
                    default:
                        return true;
                }
                break;
            case Expected::OPERATOR:
                switch(have){
                    case TokenType::ADD:
                        return true;
                        break;
                    case TokenType::SUB:
                        return true;
                        break;
                    case TokenType::MUL:
                        return true;
                        break;
                    case TokenType::DIV:
                        return true;
                        break;
                }
                break;
        }
    }
    return false;
}

void report_error_to_diagnostics(const std::vector<Token>& line, const std::vector<Expected>& expected_vector, const Token& got, std::string file_name){
    if(expected_vector== std::vector<Expected>{Expected::TYPE} && got.type== TokenType::EQUAL){
        /*
        * struct Error{
        *     size_t pos, size, line;
        *     const std::vector<Token>& org_line;
        *     std::string file_name;
        *     Code code;
        * };
        */
       std::cout << '\n';
       Report(Error{got.org_start_pos, got.org_word.size(), got.line_num, line, file_name, Code::TYPE_MISS});
    }
}

void LET(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector){
    type = AST1_Type::DECLARATION;
    expected_vector.clear();
    expected_vector = {Expected::IDENTIFIER};
}

void IDENT(const Token& token, std::string& ident, std::vector<Expected>& expected_vector){
    ident = token.org_word;
    expected_vector.clear();
    expected_vector = {Expected::COLON, Expected::EQUAL};
}   

void COLON(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::TYPE};
}

void TYPE(const Token& token, std::vector<Expected>& expected_vector, TokenType& dec_type){
    expected_vector.clear();
    expected_vector = {Expected::EQUAL};

    dec_type = token.type;
}

void EQUAL(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::LITERAL, Expected::IDENTIFIER};
}

void LITERAL(std::vector<Expected>& expected_vector, std::vector<Token>& expr, const Token& token){
    expected_vector.clear();
    expected_vector.push_back(Expected::OPERATOR);
    expr.push_back(token);
}

void OPERATOR(std::vector<Expected>& expected_vector, std::vector<Token>& expr, const Token& token){
    expected_vector.clear();
    expected_vector = {Expected::LITERAL, Expected::IDENTIFIER};
    expr.push_back(token);
}


void PRINT(const Token& token, AST1_Type& type, std::vector<Expected>& expected_vector){
    type = AST1_Type::PRINT;
    expected_vector.clear();
    expected_vector = {Expected::RPARA};
}

void RPARA(std::vector<Expected>& expected_vector){
    expected_vector.clear();
    expected_vector = {Expected::RPARA};
}

void call_token(std::vector<Token>& expr, const Token& token, AST1_Type& type, std::string& ident, std::vector<Expected>& expected_vector, TokenType& dec_type){
    if(token.type== TokenType::LET) LET(token, type, expected_vector);
    else if(token.type== TokenType::IDENTIFIER) IDENT(token, ident, expected_vector);
    else if(token.type== TokenType::COLON) COLON(expected_vector);
    else if(is_type(token.type)) TYPE(token, expected_vector, dec_type);
    else if(token.type== TokenType::EQUAL) EQUAL(expected_vector);
    else if(token.type== TokenType::INT || token.type== TokenType::FLOAT || token.type== TokenType::STR) LITERAL(expected_vector, expr, token);
    else if(is_operator(token.type)) OPERATOR(expected_vector, expr, token);
    else if(token.type== TokenType::PRINT) PRINT(token, type, expected_vector);
}

void print_expr(const ExprNode& node, const std::string& prefix = "", bool is_right = false){
    if(std::holds_alternative<LiteralExpr>(node.node)){
        std::cout << prefix << (is_right ? "└── " : "├── ");
        std::cout << std::get<LiteralExpr>(node.node).value.org_word << '\n';
    } else {
        const BinaryExpr& bin = std::get<BinaryExpr>(node.node);
        std::cout << prefix << (is_right ? "└── " : "├── ");
        std::cout << bin.op.org_word << '\n';
        std::string new_prefix = prefix + (is_right ? "    " : "    ");
        print_expr(*bin.left, new_prefix, false);
        print_expr(*bin.right, new_prefix, true);
    }
}

ExprNode Parse_expression(const std::vector<Token>& expr){
    std::unordered_map<std::string, int> operator_precedence; // greater number = greater precedence
    operator_precedence["+"] = 1;
    operator_precedence["-"] = 1;
    operator_precedence["*"] = 2;
    operator_precedence["/"] = 2;
    std::vector<Token> output_stack;
    std::vector<Token> operator_stack;
    for(const Token& token : expr){
        if(token.type== TokenType::INT || token.type== TokenType::FLOAT || token.type== TokenType::STR || token.type== TokenType::IDENTIFIER){
            output_stack.push_back(token);
        }
        else if(is_operator(token.type)){

            while(!operator_stack.empty()){
                if(operator_precedence.at(operator_stack.back().org_word) >= operator_precedence.at(token.org_word)){
                    output_stack.push_back(operator_stack.back());
                    operator_stack.pop_back();
                }
                else{
                    break;
                }
            }
            operator_stack.push_back(token);
        }
    }
    while(!operator_stack.empty()){
        output_stack.push_back(operator_stack.back());
        operator_stack.pop_back();
    }

    std::vector<ExprNode> node_stack;

    for(const Token& token : output_stack){
        if(token.type == TokenType::INT || token.type == TokenType::FLOAT || 
        token.type == TokenType::STR || token.type == TokenType::IDENTIFIER){
            node_stack.push_back(ExprNode{LiteralExpr{token}});
        }
        else if(is_operator(token.type)){
            ExprNode right = std::move(node_stack.back()); node_stack.pop_back();
            ExprNode left  = std::move(node_stack.back()); node_stack.pop_back();
            
            node_stack.push_back(ExprNode{BinaryExpr{
                token,
                std::make_unique<ExprNode>(std::move(left)),
                std::make_unique<ExprNode>(std::move(right))
            }});
        }
    }

    ExprNode root = std::move(node_stack.back());
    return root;
}

#define DEBUG

AST1_NODE AST1(const std::vector<Token>& line, std::string file_name){
    std::vector<Token> expr;
    AST1_Type type;
    TokenType dec_type;
    std::string ident;
    std::vector<Expected> expected_vector = {Expected::START};
    #ifdef DEBUG
    int z = 0;
    #endif
    for(size_t i = 0; i < line.size(); ++i){
        const Token& token = line[i];
        #ifdef DEBUG
        std::cout << int(token.type) << ' ';
        #endif
        if(is_expected(expected_vector, token.type)) call_token(expr, token, type, ident, expected_vector, dec_type);
        else{ report_error_to_diagnostics(line, expected_vector, token, file_name); return AST1_NODE{}; }
    }
    #ifdef DEBUG
    std::cout << "\nOUT:\n";
    std::cout << "AST type: " << int(type) << '\n';
    std::cout << "Declaration type: " << token_word[dec_type] << '\n';
    std::cout << "Declaration Identifier: " << ident << '\n';
    #endif
    switch(type){
        case AST1_Type::DECLARATION:
            print_expr(Parse_expression(expr));
            return AST1_NODE{AST1_Type::DECLARATION, Declaration_Node{ident, dec_type, Parse_expression(expr)}, false, true};
    }
    return AST1_NODE{};
}

int main(int argc, char** argv){
    std::cout << "Hello from [COMPILER]\n";

    std::string file_name = argv[1];
    std::ifstream read(file_name);
    std::string temp;
    std::vector<std::string> lines;
    while(std::getline(read, temp)) lines.push_back(temp);
    for(std::vector<lexical_token> lex : Stream(lines)){
        AST1(Tokenize(lex), file_name);
    }
    return 0;
}

