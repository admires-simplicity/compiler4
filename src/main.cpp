#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cstdint>
#include <optional>
#include <sstream>

#include "lexer.h"
#include "parser.h"
#include "AST.h"

int main() {
    // Token *rootToken = new Token(Token::Type::multiply, "*", 0, 0);
    // SyntaxNode *root = new SyntaxNode(rootToken);
    // root->children.push_back(new SyntaxNode(new Token(Token::Type::number, "2", 0, 0)));
    // root->children.push_back(new SyntaxNode(new Token(Token::Type::number, "3", 0, 0)));

    // proper AST for 3 > 4 + 5 * 6
    Token *root1 = new Token(Token::Type::identifier, ">", 0, 0);
    SyntaxNode *ast1 = new SyntaxNode(root1);
    ast1->children.push_back(new SyntaxNode(new Token(Token::Type::number, "3", 0, 0)));
    ast1->children.push_back(new SyntaxNode(new Token(Token::Type::identifier, "+", 0, 0)));
    ast1->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "4", 0, 0)));
    ast1->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "*", 0, 0)));
    ast1->children[1]->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "5", 0, 0)));
    ast1->children[1]->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "6", 0, 0)));

    // proper AST for 6 * 5 + 4 < 3
    Token *root2 = new Token(Token::Type::identifier, "<", 0, 0);
    SyntaxNode *ast2 = new SyntaxNode(root2);
    ast2->children.push_back(new SyntaxNode(new Token(Token::Type::identifier, "+", 0, 0)));
    ast2->children.push_back(new SyntaxNode(new Token(Token::Type::number, "3", 0, 0)));
    ast2->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::identifier, "*", 0, 0)));
    ast2->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "4", 0, 0)));
    ast2->children[0]->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "6", 0, 0)));
    ast2->children[0]->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "5", 0, 0)));

    // proper AST for 1 * 2 + 3 * 4
    Token *root3 = new Token(Token::Type::identifier, "+", 0, 0);
    SyntaxNode *ast3 = new SyntaxNode(root3);
    ast3->children.push_back(new SyntaxNode(new Token(Token::Type::identifier, "*", 0, 0)));
    ast3->children.push_back(new SyntaxNode(new Token(Token::Type::identifier, "*", 0, 0)));
    ast3->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "1", 0, 0)));
    ast3->children[0]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "2", 0, 0)));
    ast3->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "3", 0, 0)));
    ast3->children[1]->children.push_back(new SyntaxNode(new Token(Token::Type::number, "4", 0, 0)));

    Lexer lexer(std::cin);

    //std::cout << root->to_string() << std::endl;

    std::cout << ast1->to_string() << std::endl;
    std::cout << ast2->to_string() << std::endl;
    std::cout << ast3->to_string() << std::endl;

    // std::cout << binary_precedence[syntax_ids[ast1->token->literal]].precedence << std::endl;
    // std::cout << binary_precedence[syntax_ids[ast2->token->literal]].precedence << std::endl;
    // std::cout << binary_precedence[syntax_ids[ast3->token->literal]].precedence << std::endl;


    // while (lexer.nonempty()) {
    //     std::cout << lexer.next().value().literal << std::endl;
    // }

    Lexer lexer2(std::istringstream("3 > 4 + 5 * 6"));
    std::cout << "lexer2: " << std::endl;
    std::cout << Parser(lexer2).parse()->to_string() << std::endl;

    SyntaxNode *parsed = Parser(lexer).parse();
    std::cout << "\nparsed: " << std::endl;
    if (parsed) std::cout << parsed->to_string() << std::endl;
    else std::cout << "null" << std::endl;

    return 0;
}