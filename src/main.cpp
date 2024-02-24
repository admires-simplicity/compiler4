#include <iostream>
#include <vector>
#include <set>

#include "token.h"
#include "lexer.h"

int main() {
    Token *root = new Token(Token::Type::multiply, "*");    
    root->children.push_back(new Token(Token::Type::number, "2"));
    root->children.push_back(new Token(Token::Type::number, "3"));

    Lexer lexer(std::cin);

    std::set<char> syntax_chars({'(', ')', '+', '*', '.'});

    std::set<int> is { {1, 2, 3, 4} };

    //std::cout << true << std::endl;
    
    std::cout << std::set<char>({'(', ')', '+', '*', '.'}).contains('*') << std::endl;

    std::cout << root->to_string() << std::endl;



    return 0;
}